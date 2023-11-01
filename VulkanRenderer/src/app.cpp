#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "ld_camera.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "ld_buffer.hpp"
#include "ld_texture.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>

namespace Ld {
	App::App()
	{
		m_globalPool = DescriptorPool::Builder(m_device)
			.setMaxSets(SwapChain::s_maxFramesInFlight)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::s_maxFramesInFlight)
			.build();

		loadGameObjects();
	}

	App::~App()
	{
	}

	void App::run()
	{
		std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::s_maxFramesInFlight);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uint32_t instanceCount = 1;
			VmaAllocationCreateInfo uboAllocInfo{};
			uboAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

			VkDeviceSize alignmentSize = Ld::Buffer::getAlignment(sizeof(GlobalUBO), instanceCount);
			VkBufferCreateInfo bufferCreateInfo{};
			bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			uboBuffers[i] = std::make_unique<Buffer>(
				m_device,
				bufferCreateInfo,
				sizeof(GlobalUBO),
				instanceCount,
				uboAllocInfo,
				alignmentSize
			);
			if (uboBuffers[i]->map() != VK_SUCCESS)
			{
				throw std::runtime_error("couldn't map buffers");
			}
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(m_device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::s_maxFramesInFlight);

		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *m_globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ m_device, m_renderer.getSwapChainRenderPass() , globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ m_device, m_renderer.getSwapChainRenderPass() , globalSetLayout->getDescriptorSetLayout() };
		Camera camera{};

		auto viewerObject = GameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!m_window.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(m_window.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = m_renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			// beginFrame() returns nullptr if swapchain needs to be recreated
			if (auto commandBuffer = m_renderer.beginFrame()) 
			{
				int frameIndex = m_renderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					m_gameObjects
				};
				// update objects in memory
				GlobalUBO ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// additional render passes can be added here later
				//render
				m_renderer.beginSwapChainRenderPass(commandBuffer);
				
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				
				m_renderer.endSwapChainRenderPass(commandBuffer);
				m_renderer.endFrame();
			}
		}
		vkDeviceWaitIdle(m_device.device());
	}

	void App::loadGameObjects()
	{
		std::shared_ptr<Model> ldModel = Model::createModelFromFile(m_device, "models/flat_vase.obj");
		auto flatVase = GameObject::createGameObject();
		flatVase.model = ldModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f,1.5f, 3.f };
		m_gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		ldModel = Model::createModelFromFile(m_device, "models/smooth_vase.obj");
		auto smoothVase = GameObject::createGameObject();
		smoothVase.model = ldModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f,1.5f, 3.f };
		m_gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		ldModel = Model::createModelFromFile(m_device, "models/quad.obj");
		auto floor = GameObject::createGameObject();
		floor.model = ldModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = { 3.f,1.f, 3.f };
		m_gameObjects.emplace(floor.getId(), std::move(floor));

		std::vector<glm::vec3> lightColors{
			 {1.f, .1f, .1f},
			 {.1f, .1f, 1.f},
			 {.1f, 1.f, .1f},
			 {1.f, 1.f, .1f},
			 {.1f, 1.f, 1.f},
			 {1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = GameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			m_gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}

		std::unique_ptr<Texture> texture = Texture::createTextureFromFile(m_device, "textures/texture.jpg");
	}
}
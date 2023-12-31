#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "ld_camera.hpp"
#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "ld_buffer.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>
#include <array>

namespace ld {
	// be aware of alignment rules std140


	App::App()
	{
		globalPool = LdDescriptorPool::Builder(ldDevice)
			.setMaxSets(LdSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LdSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		loadGameObjects();
	}

	App::~App()
	{
	}

	void App::run()
	{
		std::vector<std::unique_ptr<LdBuffer>> uboBuffers(LdSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<LdBuffer>(
				ldDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = LdDescriptorSetLayout::Builder(ldDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(LdSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			LdDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);


		}


		SimpleRenderSystem simpleRenderSystem{ ldDevice, ldRenderer.getSwapChainRenderPass() , globalSetLayout->getDescriptorSetLayout() };
		PointLightSystem pointLightSystem{ ldDevice, ldRenderer.getSwapChainRenderPass() , globalSetLayout->getDescriptorSetLayout() };
		LdCamera camera{};

		auto viewerObject = LdGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};


		auto currentTime = std::chrono::high_resolution_clock::now();
		while (!ldWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(ldWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = ldRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			// beginFrame() returns nullptr if swapchain needs to be recreated
			if (auto commandBuffer = ldRenderer.beginFrame()) {
				int frameIndex = ldRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
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
				ldRenderer.beginSwapChainRenderPass(commandBuffer);
				
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				
				ldRenderer.endSwapChainRenderPass(commandBuffer);
				ldRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(ldDevice.device());
	}



	void App::loadGameObjects()
	{
		std::shared_ptr<LdModel> ldModel = LdModel::createModelFromFile(ldDevice, "models/flat_vase.obj");
		auto flatVase = LdGameObject::createGameObject();
		flatVase.model = ldModel;
		flatVase.transform.translation = { -.5f, .5f, 0.f };
		flatVase.transform.scale = { 3.f,1.5f, 3.f };
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		ldModel = LdModel::createModelFromFile(ldDevice, "models/smooth_vase.obj");
		auto smoothVase = LdGameObject::createGameObject();
		smoothVase.model = ldModel;
		smoothVase.transform.translation = { .5f, .5f, 0.f };
		smoothVase.transform.scale = { 3.f,1.5f, 3.f };
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		ldModel = LdModel::createModelFromFile(ldDevice, "models/quad.obj");
		auto floor = LdGameObject::createGameObject();
		floor.model = ldModel;
		floor.transform.translation = { 0.f, .5f, 0.f };
		floor.transform.scale = { 3.f,1.f, 3.f };
		gameObjects.emplace(floor.getId(), std::move(floor));

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
			auto pointLight = LdGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
}
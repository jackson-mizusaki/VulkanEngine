#include "ld_renderer.hpp"


#include <stdexcept>
#include <array>

namespace ld {


	LdRenderer::LdRenderer(LdWindow &window, LdDevice & device) : ldWindow{window}, ldDevice{device}
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	LdRenderer::~LdRenderer()
	{
		freeCommandBuffers();
	}

	VkCommandBuffer LdRenderer::beginFrame()
	{
		assert(!isFrameStarted && "can't call beginFrame while already in progress");		
		auto result = ldSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			// can occur when window is resized
			throw std::runtime_error("failed to acquire swap chain image");
		}

		isFrameStarted = true;
		 
		auto commandBuffer = getCurrentCommandBuffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer");
		}
		return commandBuffer;
	}

	void LdRenderer::endFrame()
	{
		assert(isFrameStarted && "can't call endframe while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer");
		}

		auto result = ldSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || ldWindow.wasWindowResized())
		{
			ldWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % LdSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void LdRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainREnderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ldSwapChain->getRenderPass();
		renderPassInfo.framebuffer = ldSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = ldSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(ldSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(ldSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, ldSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void LdRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainREnderPass if frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	void LdRenderer::createCommandBuffers()
	{
		commandBuffers.resize(LdSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = ldDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(ldDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers");
		}
	}


	void LdRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(ldDevice.device(), ldDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}



	void LdRenderer::recreateSwapChain()
	{
		auto extent = ldWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = ldWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(ldDevice.device());

		if (ldSwapChain == nullptr)
		{
			ldSwapChain = std::make_unique<LdSwapChain>(ldDevice, extent);
		}
		else
		{
			std::shared_ptr<LdSwapChain> oldSwapChain = std::move(ldSwapChain);
			ldSwapChain = std::make_unique<LdSwapChain>(ldDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*ldSwapChain.get())) 
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}

		}
		// if render pass compatible, do nothing.
	}


}
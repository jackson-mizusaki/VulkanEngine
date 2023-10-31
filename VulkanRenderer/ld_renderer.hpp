#pragma once

#include "ld_window.hpp"
#include "ld_device.hpp"
#include "ld_swapchain.hpp"
#include "ld_model.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace ld {
	class LdRenderer {
	public:
		LdRenderer(LdWindow& window, LdDevice& device);
		~LdRenderer();

		LdRenderer(const LdRenderer&) = delete;
		LdRenderer& operator=(const LdRenderer&) = delete;
	private:
		LdWindow& ldWindow;
		LdDevice& ldDevice;
		std::unique_ptr<LdSwapChain> ldSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;


		uint32_t currentImageIndex;
		int currentFrameIndex; // from 0 - max_frames_in_flight
		bool isFrameStarted;

	public:
		VkRenderPass getSwapChainRenderPass() const { return ldSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const 
		{
			assert(isFrameStarted && "Cannot get command buffer when frame not i n progress");
			return commandBuffers[currentFrameIndex];
		}
		int getFrameIndex() const 
		{
			assert(isFrameStarted && "Cannot get command buffer when frame not i n progress");
			return currentFrameIndex;
		}
		float getAspectRatio() const { return ldSwapChain->extentAspectRatio(); }
		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
	};
}

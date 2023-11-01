#pragma once

#include "ld_window.hpp"
#include "ld_device.hpp"
#include "ld_swapchain.hpp"
#include "ld_model.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace Ld {
	class LdRenderer {
	public: // constructors
		LdRenderer(LdWindow& window, LdDevice& device);
		~LdRenderer();
		LdRenderer(const LdRenderer&) = delete;
		LdRenderer& operator=(const LdRenderer&) = delete;

	public: // functions
		VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }
		bool isFrameInProgress() const { return m_isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const
		{
			assert(m_isFrameStarted && "Cannot get command buffer when frame not i n progress");
			return m_commandBuffers[m_currentFrameIndex];
		}
		int getFrameIndex() const
		{
			assert(m_isFrameStarted && "Cannot get command buffer when frame not i n progress");
			return m_currentFrameIndex;
		}
		float getAspectRatio() const { return m_swapChain->extentAspectRatio(); }
		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

	public: // data
	private:
		LdWindow& m_window;
		LdDevice& m_device;
		std::unique_ptr<LdSwapChain> m_swapChain;
		std::vector<VkCommandBuffer> m_commandBuffers;

		uint32_t m_currentImageIndex;
		int m_currentFrameIndex; // from 0 - max_frames_in_flight
		bool m_isFrameStarted;
	};
}

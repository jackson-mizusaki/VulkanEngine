#pragma once

#include "ld_camera.hpp"
#include "ld_pipeline.hpp"
#include "ld_device.hpp"
#include "ld_game_object.hpp" 
#include "ld_frame_info.hpp"

#include <memory>
#include <vector>

namespace Ld {
	class SimpleRenderSystem {
	public: // constructors
		SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	public: // functions
		void renderGameObjects(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

	public: // data
	private:
		Device &m_device;
		std::unique_ptr<Pipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;
	};
}

#pragma once

#include "ld_camera.hpp"
#include "ld_pipeline.hpp"
#include "ld_device.hpp"
#include "ld_game_object.hpp" 
#include "ld_frame_info.hpp"

#include <memory>
#include <vector>

namespace ld {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(LdDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();
		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
	
	private:
		LdDevice &ldDevice;
		std::unique_ptr<LdPipeline> ldPipeline;
		VkPipelineLayout pipelineLayout;

	public:
		void renderGameObjects(FrameInfo &frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};
}

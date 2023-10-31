#pragma once

#include "ld_camera.hpp"
#include "ld_pipeline.hpp"
#include "ld_device.hpp"
#include "ld_game_object.hpp" 
#include "ld_frame_info.hpp"

#include <memory>
#include <vector>

namespace ld {
	class PointLightSystem {
	public:
		PointLightSystem(LdDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

	private:
		LdDevice& ldDevice;
		std::unique_ptr<LdPipeline> ldPipeline;
		VkPipelineLayout pipelineLayout;

	public:
		void render(FrameInfo& frameInfo);
		void update(FrameInfo& frameInfo, GlobalUBO& ubo);
	
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
	};
}

#pragma once

#include "ld_camera.hpp"
#include "ld_pipeline.hpp"
#include "ld_device.hpp"
#include "ld_game_object.hpp" 
#include "ld_frame_info.hpp"

#include <memory>
#include <vector>

namespace Ld {
	class PointLightSystem {
	public: // constructors
		PointLightSystem(LdDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

	public: // functions
		void render(FrameInfo& frameInfo);
		void update(FrameInfo& frameInfo, GlobalUBO& ubo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

	public: // data
	private:
		LdDevice& m_device;
		std::unique_ptr<LdPipeline> m_pipeline;
		VkPipelineLayout m_pipelineLayout;

	};
}

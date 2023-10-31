#pragma once

#include "ld_camera.hpp"
#include "ld_game_object.hpp"


#include <vulkan/vulkan.h>

namespace ld {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LdCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		LdGameObject::Map& gameObjects;
	};
}
#pragma once

#include "ld_camera.hpp"
#include "ld_game_object.hpp"


#include <vulkan/vulkan.h>


namespace ld {
#define MAX_LIGHTS 10

	struct PointLight {
		glm::vec4 position{}; // ignore w
		glm::vec4 color{}; // w is intensity
	};

	struct GlobalUBO {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 0.9f, 1.f, .02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LdCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		LdGameObject::Map& gameObjects;
	};	
}
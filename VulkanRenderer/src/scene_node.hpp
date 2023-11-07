#pragma once

#include "mesh.hpp"
#include "ld_camera.hpp"
#include "skin.hpp"
#include "node.hpp"

#include <json.hpp>
#include <glm/glm.hpp>

#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class SceneNode : public Node {
	public: // types
		using id_t = unsigned int;
	public: // contructors
		SceneNode();
	public: // functions
		void render(VkCommandBuffer commandBuffer);
		bool findCamera(SceneNode& cam);
	public: // data
		Camera* camera = nullptr;
		Skin* skin = nullptr;
		Mesh* mesh = nullptr;
		uint32_t meshMorphTargetCount = 0;
	private:
		id_t m_id;
	};
}
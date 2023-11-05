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
	public: // constructors
		SceneNode();

	public: // data
		Camera* camera = nullptr;
		Skin* skin = nullptr;
		Mesh* mesh = nullptr;
		uint32_t meshMorphTargetCount;
	};
}
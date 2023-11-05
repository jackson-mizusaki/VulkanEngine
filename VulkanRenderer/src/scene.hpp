#pragma once

#include "ld_camera.hpp"
#include "animation.hpp"
#include "scene_node.hpp"


#include <json.hpp>
#include <glm/glm.hpp>

#include <string>
#include <vector>

using json = nlohmann::json;


namespace Ld {
	class Scene {
	public:	// types


	public: // constructors
		Scene();
		Scene(std::string& filepath);
		~Scene() = default;               // destructor 
		Scene(const Scene&) = default;                // copy constructor
		Scene& operator=(const Scene&) = default;     // copy assignment
		Scene(Scene&&) noexcept = default;            // move constructor
		Scene& operator=(Scene&&) noexcept = default; // move assignment
 
	public: // functions
		void loadScene(const std::string& filepath);

	public: // data
		std::vector<SceneNode> nodes;
		std::string name;
		json extensions;
		json extras;
	};
}
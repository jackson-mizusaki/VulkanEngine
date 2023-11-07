#pragma once  

#include "meshprimitive.hpp"

#include <json.hpp>


#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Mesh {
	public: // functiosn
		void render(VkCommandBuffer commandBuffer);
	public: // data
		std::vector<MeshPrimitive> primitives;
		int morphTargetCount = 0;
	};
}

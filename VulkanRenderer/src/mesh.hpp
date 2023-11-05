#pragma once  

#include "ld_model.hpp"

#include <json.hpp>


#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Mesh {
	public:
		std::vector<Model> primitives;
		int morphTargetCount = 0;
	};
}

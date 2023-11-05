#pragma once

#include "bone.hpp"	

#include <json.hpp>

#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Skin {
	public: // data
		Bone root;
		std::string name;

	};
}
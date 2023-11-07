#pragma once

#include "ld_buffer.hpp"
#include "gltf_importer.hpp"

#include <json.hpp>


#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Accessor {
	public: // Types

		// todo figure out sparse accessors
	public: // constructors
		//Accessor();
		Accessor(Device& device);
	public: // functions
		void loadBuffer(const std::vector<uint8_t>& data, VkDeviceSize size);
	public: // data
		Device& m_device;
		uint32_t bufferViewIndex;
		uint32_t binding = 0;
		uint32_t byteOffset = 0;
		std::vector<float> maxes;
		std::vector<float> mins;
		ComponentType componentType = Byte;
		bool isNormalized = false;
		uint32_t count = 0;
		Type accessorType = Scalar;

	};
}
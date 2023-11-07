#pragma once

#include "ld_buffer.hpp"

#include <json.hpp>


#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Accessor {
	public: // Types
		enum Type {
			Scalar,
			Vec2,
			Vec3,
			Vec4,
			Mat2,
			Mat3,
			Mat4
		};
		enum ComponentType {
			Byte,
			Unsigned_Byte,
			Short,
			Unsigned_Short,
			Unsigned_Int,
			Float
		};
		// todo figure out sparse accessors
	public: // constructors
		//Accessor();
		Accessor(Device& device);
	public: // functions
		void loadBuffer();
	public: // data
		Device& m_device;
		Buffer* buffer = nullptr;
		uint32_t binding = 0;
		VkBufferView bufferView;
		uint32_t byteOffset = 0;
		std::vector<float> Maxes;
		std::vector<float> Mins;
		ComponentType componentType = Byte;
		bool isNormalized = false;
		uint32_t count = 0;
		Type accessorType = Scalar;

	};
}
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
		class Sparse {
		public: // types
			class Index {
			public: // types
				enum ComponentType {
					Unsigned_Byte,
					Unsigned_Short,
					Unsigned_Int
				};
				uint32_t bufferViewIndex;
				uint32_t byteOffset;
				ComponentType componentType;
				//json extensions;
				//json extras;
			};
			class Value {
				uint32_t bufferViewIndex;
				uint32_t byteOffset;
				//json extensions;
				//json extras;
			};
		public: // data
			uint32_t count;
			std::vector<Index> indicies;
			std::vector<Value> values;
			//json extensions;
			//json extras;
		};

	public: // constructors
		Accessor();
	public: // functions
		Buffer* loadBuffer();
	public: // data
		uint32_t bufferViewIndex;
		uint32_t byteOffset = 0;
		std::vector<float> Maxes;
		std::vector<float> Mins;
		ComponentType componentType;
		bool isNormalized = false;
		uint32_t count;
		Type accessorType;

	};
}
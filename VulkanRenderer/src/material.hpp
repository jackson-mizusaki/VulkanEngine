#pragma once 

#include <json.hpp>
#include "ld_texture.hpp"

#include <string>
#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Material {
	public: // types
		class PBRMetallicRoughness {
			std::vector<float> baseColorFactor{ 1.f, 1.f, 1.f, 1.f };
			Texture baseColorTexture;
			float metallicFactor{ 1.f };
			float roughnessFactor{ 1.f };
			Texture metallicRoughnessTexture;
			//json extensions;
			//json extras;
		};
		class NormalTexture {
			Texture normalTexture;
			float scale{ 1.f };
			//json extensions;
			//json extras;
		};
		class OcclusionTexture {
			Texture occlusionTexture;
			float strength{ 1.f };
			//json extensions;
			//json extras;
		};
		class EmissiveTexture {
			Texture emissiveTexture;
			std::vector<float> emissiveFactor{ 0.f, 0.f, 0.f };
		};
		enum AlphaMode {
			Opaque,
			Mask, // depends on alphacutoff
			Blend
		};
	public: // data
		std::string name;

		PBRMetallicRoughness pbrMetallicRoughness;
		NormalTexture normalTexture;
		OcclusionTexture occlusionTexture;
		AlphaMode alphaMode{ Opaque };
		float alphaCutoff{ 0.5f };
		bool isDoubleSided = false;
		//json extensions;
		//json extras;
	};
}
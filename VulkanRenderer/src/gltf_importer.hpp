#pragma once 

#include "app.hpp"
#include "accessor.hpp"
#include "scene.hpp"
#include "material.hpp"
#include "animation.hpp"
#include "meshprimitive.hpp"
#include "skin.hpp"
#include "ld_buffer.hpp"
#include "glmjson.hpp"

#include <json.hpp>

#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class GlTFImporter {
	public: // types
		// Buffer Data types need to be passed into structs first before accessors can work with them
		struct GlTFBuffer {
			std::string uri;
			uint32_t byteLength;
			std::string name;
			std::vector<std::uint8_t> data;
			//json extensions
			//json extras
		};
		struct GlTFBufferView {
			enum BufferViewTarget {
				Array_Buffer,
				Element_Array_Buffer
			};
			uint32_t bufferIndex;
			uint32_t byteOffset = 0;
			uint32_t byteLength;
			uint32_t byteStride;
			BufferViewTarget target;
			std::string name;
			//json extensions
			//json extras
		};		
		struct GlTFImage {
			std::string uri;
			GlTFBufferView* bufferView;
			std::string mimeType;
			std::string name;
			//json extensions
			//json extras
		};
		struct GlTFTexture {
			GlTFImage* image;
			uint32_t samplerIndex;
			std::string name;
			//json extensions
			//json extras
		};

	public: // constructors
		GlTFImporter(Device &device, const std::string& filepath, std::vector<Scene>& scenes);
	public: // functions

	private:
		void loadBuffer(GlTFBuffer & buffer, json bufferDatas);
		void loadBufferView(GlTFBufferView& bufferView, json bufferViewData);
		void loadImage(GlTFImage& image, json imageData);
		void loadAccessor(Accessor& accessor, json accessorData);
		void loadSceneNode(SceneNode& sceneNode, json nodeData, uint32_t i);
		void loadCamera(Camera& camera, json cameraData);
		void loadMesh(Mesh& mesh, json meshData);
		void loadSkin(Skin& skin, json skinData);
		void loadTexture(Texture& texture, json textureData);
		void loadMaterial(Material& material, json materialData);
		void loadAnimation(Animation& animation, json animData);
		void loadScene(Scene& scene, json sceneData);
	private: // data
		Device& m_device;
		std::map<uint32_t, std::vector<uint32_t>> nodeChildren;
		std::vector<SceneNode*> sceneNodes;
		std::vector<Camera*> cameras;
		std::vector<Skin*> skins;
		std::vector<Mesh*> meshes;
		std::vector<Accessor*> accessors;
		std::vector<GlTFImage*> images;
		std::vector<Texture::Sampler*> samplers;
		std::vector<GlTFBuffer*> buffers;
		std::vector<GlTFBufferView*> bufferViews;
	};
}

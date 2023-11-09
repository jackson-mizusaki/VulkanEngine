#pragma once 

#include "app.hpp"
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
			uint32_t byteLength = 0;
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
			uint32_t bufferIndex = 0;
			uint32_t byteOffset = 0;
			VkDeviceSize byteLength = 0;
			uint32_t byteStride = 0;
			BufferViewTarget target;
			std::string name;
			//json extensions
			//json extras
		};		
		struct GlTFAccessor {
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
			ComponentType componentType;
			Type accessorType;
			std::vector<float> maxes;
			std::vector<float> mins;
			uint32_t bufferViewIndex = 0;
			uint32_t byteOffset = 0;
			uint32_t count = 0;
		};
		struct GlTFImage {
			std::string uri;
			uint32_t bufferViewIndex;
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
		void loadAccessor(GlTFAccessor& accessor, json accessorData);
		void loadSceneNode(SceneNode& sceneNode, json nodeData, uint32_t i);
		void loadCamera(Camera& camera, json cameraData);
		void loadMesh(Mesh& mesh, json meshData);
		void loadSkin(Skin& skin, json skinData);
		void loadTexture(Texture& texture, json textureData);
		void loadMaterial(Material& material, json materialData);
		void loadAnimation(Animation& animation, json animData);
		void loadScene(Scene& scene, json sceneData);
	private: // data
		std::string m_filepath;
		Device& m_device;
		std::map<uint32_t, std::vector<uint32_t>> nodeChildren;
		std::vector<SceneNode> sceneNodes;
		std::vector<Camera> cameras;
		std::vector<Skin> skins;
		std::vector<Mesh> meshes;
		std::vector<GlTFAccessor> accessors;
		std::vector<GlTFImage> images;
		std::vector<Texture::Sampler*> samplers;
		std::vector<GlTFBuffer> buffers;
		std::vector<GlTFBufferView> bufferViews;
	};
}

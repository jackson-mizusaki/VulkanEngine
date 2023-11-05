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
	public: // constructors
		GlTFImporter(App& app, std::string& filepath);
	public: // functions
	private:
		void loadCamera(Camera& camera, json cameraData);
		void loadBuffers(json bufferData);
		void loadBufferViews(json bufferViewData);
		void loadAccessors(json accessorData);
		void loadScene(Scene& scene, json sceneData);
		void loadSceneNode(SceneNode& sceneNode, json nodeData);
		void loadMesh(Mesh& mesh, json meshData);
		void loadSkin(Skin& skin, json skinData);
		void loadTexture(Texture& texture, json textureData);
		void loadMaterial(Material& material, json matData);
		void loadAnimation(Animation& animation, json animData);
	private: // data
		std::vector<SceneNode*> sceneNodes;
		std::vector<Camera*> cameras;
		std::vector<Skin*> skins;
		std::vector<Mesh*> meshes;
		std::vector<Accessor> accessors;
		std::vector<GlTFBuffer*> buffers;
		std::vector<GlTFBufferView> bufferViews;
	};
}

#include "gltf_importer.hpp"

#include "base64.hpp"

#include <memory>
#include <fstream>
#include <stdexcept>

namespace Ld {
	GlTFImporter::GlTFImporter(App& app, std::string& filepath)
	{
		std::ifstream f(filepath.c_str());
		json j;
		f >> j;

		if (!j.contains("asset"))
		{
			throw std::runtime_error("Asset is missing, gltf is invalid");
		}
		if (j.contains("scenes"))
		{
			if (j.contains("scene")) {
				if (j["scene"] > j["scenes"].size() - 1)
				{
					throw std::runtime_error("Default scene index exceeds number of scenes");
				}
				app.setDefaultScene(j["scene"]);
			}
		}
		for (json bufferData : j.at("buffers"))
		{
			loadBuffers(bufferData);
		}
		for (json bufferViewData : j.at("bufferViews"))
		{
			loadBufferViews(bufferViewData);
		}
		
		for (json accessorData : j["accessors"])
		{
			loadAccessors(accessorData);
		}
		for (json meshData : j["meshes"])
		{
			Mesh mesh;
			loadMesh(mesh, meshData);
			meshes.push_back(&mesh);
		}
		for (json cameraData : j["cameras"])
		{
			Camera camera;
			loadCamera(camera, cameraData);
			cameras.push_back(&camera);
		}
		for (json skinData : j["skins"])
		{
			Skin skin;
			loadSkin(skin, skinData);
			skins.push_back(&skin);
		}
		for (json node : j["nodes"])
		{
			SceneNode newNode;
			loadSceneNode(newNode, node);
			sceneNodes.push_back(&newNode);
		}

		for (json s : j["scenes"])
		{
			Scene newScene{};
			loadScene(newScene, s);
			app.addScene(newScene);
		}

	}
	void GlTFImporter::loadBuffers(json buffersData)
	{
		for (json bufferData : buffersData)
		{
			if (!bufferData.contains("byteLength") || !bufferData.at("byteLength").is_number())
			{
				throw std::runtime_error("buffer does not contain required properties!");
			}
			GlTFBuffer buffer;
			buffer.byteLength = bufferData.at("byteLength");
			if (bufferData.contains("name"))
			{
				buffer.name = bufferData.at("name");
			}

			if (bufferData.contains("uri")) 
			{
				std::string URI = bufferData.at("uri");
				std::string base64Data = "";
				std::size_t found = URI.find("data:application/octet-stream;base64,");
				if (found == 0)
				{
					base64Data = URI.substr(37, std::string::npos);
					buffer.data = base64::decode(base64Data);
					// found binary data
				}
				else
				{
					found = URI.find("data:application/gltf-buffer;base64,");
					if (found  == 0)
					{
						base64Data = URI.substr(36, std::string::npos);
						buffer.data = base64::decode(base64Data);
						// found binary data
					}
					else
					{
						// read from uri file
						std::ifstream bin{ URI, std::ios::ate | std::ios::binary };

						if (!bin.is_open())
						{
							throw std::runtime_error("failed to open file: " + URI);
						}

						size_t fileSize = static_cast<size_t>(bin.tellg());
						buffer.data.resize(fileSize);

						bin.seekg(0);
						bin.read(reinterpret_cast<char*>(buffer.data.data()), fileSize);

						bin.close();
					}
				}
				// data should be full to the brim with ... data
				// do some error checkign
				assert(static_cast<uint32_t>(buffer.data.size()) == buffer.byteLength && "Buffer read does not match the size specified");
			}
			buffers.push_back(&buffer);
		}
	}

	void GlTFImporter::loadBufferViews(json bufferViewsData)
	{
		for (json bufferViewData : bufferViewsData) 
		{
			GlTFBufferView bufferView{};
			if (!bufferViewData.contains("buffer") || !bufferViewData.contains("byteLength"))
			{
				throw std::runtime_error("Buffer Views properties are not valid!");
			}
			bufferView.bufferIndex = bufferViewData.at("buffer");
			if (bufferViewData.contains("byteOffset"))
			{
				bufferView.byteOffset = bufferViewData.at("byteOffset");
			}
			bufferView.byteLength = bufferViewData.at("byteLength");
			if (bufferViewData.contains("byteStride")) 
			{
				bufferView.byteStride = bufferViewData.at("byteStride");
			}
			if (bufferViewData.contains("target"))
			{
				int targetType = bufferViewData.at("target");
				switch (targetType)
				{
				case 34962: 
					bufferView.target = GlTFBufferView::Array_Buffer;
					break;
				case 34963:
					bufferView.target = GlTFBufferView::Element_Array_Buffer;
					break;
				default:
					throw std::runtime_error("Target type is invalid!");
				}
			}
			if (bufferViewData.contains("name"))
			{
				bufferView.name = bufferViewData.at("name");
			}

			bufferViews.push_back(bufferView);
		}
	}


	void GlTFImporter::loadAccessors(json accessorData)
	{
		if (!accessorData.contains("componentType") || !accessorData.contains("count") || !accessorData.contains("type"))
		{
			throw std::runtime_error("Accessor is missing required properties.");
		}
		if (accessorData.contains("sparse"))
		{
			// do sparse accessor functionality
		}
		Accessor accessor;
		int componentType = accessorData.at("componentType");
		int byteSize = 0;
		switch (componentType)
		{
		case 5120: // byte
			accessor.componentType = Ld::Accessor::ComponentType::Byte;
			byteSize = 1;
			break;
		case 5121: // unsigned_byte
			accessor.componentType = Ld::Accessor::ComponentType::Unsigned_Byte;
			byteSize = 1;
			break;
		case 5122: // short
			accessor.componentType = Ld::Accessor::ComponentType::Short;
			byteSize = 2;
			break;
		case 5123: // unsigned_short
			accessor.componentType = Ld::Accessor::ComponentType::Unsigned_Short;
			byteSize = 2;
			break;
		case 5125: // unsigned_int
			accessor.componentType = Ld::Accessor::ComponentType::Unsigned_Int;
			byteSize = 4;
			break;
		case 5126: // float
			accessor.componentType = Ld::Accessor::ComponentType::Float;
			byteSize = 4;
			break;
		default:
			throw std::runtime_error("Component type is not valid!");
		}
		std::string type = accessorData.at("type");
		int elementSize = 0;
		int componentCount = 0;
		if (type == "SCALAR")
		{
			accessor.accessorType = Accessor::Type::Scalar;
			componentCount = 1;
		}
		else if (type == "VEC2")
		{
			accessor.accessorType = Accessor::Type::Vec2;
			componentCount = 2;
		}
		else if (type == "VEC3")
		{
			accessor.accessorType = Accessor::Type::Vec3;
			componentCount = 3;
		}
		else if (type == "VEC4")
		{
			accessor.accessorType = Accessor::Type::Vec4;
			componentCount = 4;
		}
		else if (type == "MAT2")
		{
			accessor.accessorType = Accessor::Type::Mat2;
			componentCount = 4;
		}
		else if (type == "MAT3")
		{
			accessor.accessorType = Accessor::Type::Mat3;
			componentCount = 9;
		}
		else if (type == "MAT4")
		{
			accessor.accessorType = Accessor::Type::Mat4;
			componentCount = 16;
		}
		else {
			throw std::runtime_error("Accessor Type not valid!");
		}
		accessor.Maxes.resize(componentCount);
		accessor.Mins.resize(componentCount);
		elementSize = byteSize * componentCount;


	}
	void GlTFImporter::loadScene(Scene& scene, json sceneData)
	{
	}

	void GlTFImporter::loadSceneNode(SceneNode& sceneNode, json nodeData)
	{
		bool isMeshDefined = false;
		// attach mesh if it exists
		if (nodeData.contains("mesh"))
		{
			isMeshDefined = true;
			int meshIndex = nodeData["mesh"];
			if (meshIndex > cameras.size() - 1)
			{
				throw std::runtime_error("mesh index out of range.");
			}
			else
			{
				sceneNode.mesh = meshes[meshIndex];

			}
		}
		// attach camera if it exists
		if (nodeData.contains("camera"))
		{
			int cameraIndex = nodeData["camera"];
			if (cameraIndex > cameras.size() - 1)
			{
				throw std::runtime_error("camera index out of range.");
			}
			else
			{
				sceneNode.camera = cameras[cameraIndex];
			}
		}
		// attach skin if it exists
		if (nodeData.contains("skin"))
		{
			int skinIndex = nodeData["skin"];
			if (skinIndex > skins.size() - 1)
			{
				throw std::runtime_error("skin index out of range.");
			}
			else
			{
				sceneNode.skin = skins[skinIndex];
			}
		}

		// fill out node's transform, if it exists
		if (nodeData.contains("scale"))
		{
			sceneNode.transform.scale = nodeData.at("scale");
		}
		if (nodeData.contains("rotation"))
		{
			// incoming rotation is a quaternion
			sceneNode.transform.quaternion = nodeData.at("rotation");
		}
		if (nodeData.contains("translation"))
		{
			sceneNode.transform.translation = nodeData.at("translation");
		}
		if (nodeData.contains("matrix"))
		{
			// todo figure out how to handle this
		}
		if (isMeshDefined && nodeData.contains("weights"))
		{
			assert(nodeData.at("weights").size() == sceneNode.meshMorphTargetCount && "Mesh morph targets and weights are not the same size");
		}
	}

	void GlTFImporter::loadMesh(Mesh& mesh, json meshData)
	{
		for (json primData : meshData.at("primitives"))
		{
			if (!primData.contains("attributes"))
			{
				throw std::runtime_error("Primitive does not contain required attributes object");
			}
			// create primitive using a MeshPrimtiive Builder
			MeshPrimitive::Builder builder{};
			// get the material
			if (primData.contains("mode")) {

			}
			if (primData.contains("indicies")) {
				builder.loadIndices(accessors[primData.at("indices")]);
			}
			// for each attribute
			for (json attribute : primData.at("attributes"))
			{
				if (attribute.at("POSITION").is_number())
				{
					// vertices are at this accessor:
					builder.loadVertices(accessors[attribute.at("POSITION")]);
				}

				// string manipulation to pull out accessor indicies
			}

			// using the accessor load the mesh primitive data

			// create the Mesh Primitive
			std::unique_ptr<MeshPrimitive> prim = std::make_unique<MeshPrimitive>(builder);
		}
	}

	void GlTFImporter::loadCamera(Camera& camera, json cameraData)
	{
	}

	void GlTFImporter::loadSkin(Skin& skin, json skinData)
	{
		// Skins are harder to implement, come back after you've tackled meshes
	}
	void GlTFImporter::loadTexture(Texture& texture, json textureData)
	{
	}

	void GlTFImporter::loadMaterial(Material& material, json matData)
	{
	}
}

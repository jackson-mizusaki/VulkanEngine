#include "gltf_importer.hpp"

#include "base64.hpp"

#include <memory>
#include <fstream>
#include <stdexcept>

namespace Ld {
	GlTFImporter::GlTFImporter(Device& device, const std::string& filepath, std::vector<Scene>& scenes)
		: m_device{ device }
	{
		std::ifstream f(filepath.c_str());
		json j;
		f >> j;

		if (!j.contains("asset"))
		{
			throw std::runtime_error("Asset is missing, gltf is invalid");
		}
		for (json bufferData : j.at("buffers"))
		{
			GlTFBuffer buffer;
			loadBuffer(buffer, bufferData);
			buffers.push_back(&buffer);
		}
		for (json bufferViewData : j.at("bufferViews"))
		{
			GlTFBufferView bufferView;
			loadBufferView(bufferView, bufferViewData);
			bufferViews.push_back(&bufferView);
		}
		for (json imageData : j.at("images"))
		{
			GlTFImage image;
			loadImage(image, imageData);
			images.push_back(&image);
		}
		for (json accessorData : j["accessors"])
		{
			Accessor accessor{ m_device };
			loadAccessor(accessor, accessorData);
			accessors.push_back(&accessor);
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
		for (auto& [index, nodeData] : j.at("nodes").items())
		{
			SceneNode newNode;
			loadSceneNode(newNode, nodeData, std::stoi(index));
			sceneNodes.push_back(&newNode);
		}
		int defaultScene = 0;
		if (j.contains("scenes"))
		{
			if (j.contains("scene")) {
				if (j["scene"] > j["scenes"].size() - 1)
				{
					throw std::runtime_error("Default scene index exceeds number of scenes");
				}
			}
			defaultScene = j.at("scene");
			Scene newScene{};
			loadScene(newScene, j.at("scenes")[defaultScene]);
			scenes.push_back(newScene);
			j.at("scenes").erase(defaultScene);
		}
		for (json s : j["scenes"])
		{
			Scene newScene{};
			loadScene(newScene, s);
			scenes.push_back(newScene);
		}

	}
	void GlTFImporter::loadBuffer(GlTFBuffer& buffer, json bufferData)
	{
		if (!bufferData.contains("byteLength") || !bufferData.at("byteLength").is_number())
		{
			throw std::runtime_error("buffer does not contain required properties!");
		}
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
				if (found == 0)
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
	}

	void GlTFImporter::loadBufferView(GlTFBufferView& bufferView, json bufferViewData)
	{
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
	}

	void GlTFImporter::loadImage(GlTFImage& image, json imageData)
	{
		if (imageData.contains("bufferView"))
		{
			if (imageData.contains("uri"))
			{
				throw std::runtime_error("URI and bufferView cannot both be defined");
			}
			if (!imageData.contains("mimeType"))
			{
				throw std::runtime_error("mimeType must be specified when bufferview is defined");
			}
			image.bufferView = bufferViews.at(imageData.at("bufferView"));
			image.mimeType = imageData.at("mimeType");
		}
		else {
			image.uri = imageData.at("uri");
			// TODO check if this is a data uri or a regular file
		}

	}

	void GlTFImporter::loadAccessor(Accessor& accessor, json accessorData)
	{
		if (!accessorData.contains("componentType") || !accessorData.contains("count") || !accessorData.contains("type"))
		{
			throw std::runtime_error("Accessor is missing required properties.");
		}
		if (accessorData.contains("bufferView"))
		{
			GlTFBufferView* bufferView = bufferViews.at(accessorData.at("bufferVew"));

		}
		accessor.count = accessorData.at("count");
		if (accessorData.contains("sparse"))
		{
			// do sparse accessor functionality
		}
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


	void GlTFImporter::loadMesh(Mesh& mesh, json meshData)
	{
		using Builder = MeshPrimitive::Builder;
		for (json primData : meshData.at("primitives"))
		{
			if (!primData.contains("attributes"))
			{
				throw std::runtime_error("Primitive does not contain required attributes object");
			}
			// for each attribute
			// create primitive using a MeshPrimtiive Builder
			MeshPrimitive::Builder builder{ };
			for (auto& [key, value] : primData.at("attributes").items())
			{
				if (key.compare("POSITION") == 0)
				{
					// value is the accessor that defines the vertices
					builder.positionsAccessor = accessors.at(value);
				}
				if (key.compare("NORMAL") == 0)
				{
					// value is the accessor that defines the normals
					builder.normalsAccessor = accessors.at(value);
				}
				if (key.compare("TANGENT") == 0)
				{
					// value is the accessor that defines the tangents
					builder.tangentsAccessor = accessors.at(value);
				}
				if (key.compare("TEXCOOR_") > 0)
				{
					// value is the accessor that defines the Texcoords
					builder.texCoordsAccessor.emplace(std::stoi(key.substr(8)), accessors.at(value));
				}
				if (key.compare("COLOR_") > 0)
				{
					// value is the accessor that defines the color
					builder.colorsAccessor.emplace(std::stoi(key.substr(6)), accessors.at(value));
				}
				if (key.compare("JOINTS_") > 0)
				{
					// value is the accessor that defines the joints
					builder.jointsAccessor.emplace(std::stoi(key.substr(7)), accessors.at(value));
				}
				if (key.compare("WEIGHTS_") > 0)
				{
					// value is the accessor that defines the weights
					builder.weightsAccessor.emplace(std::stoi(key.substr(8)), accessors.at(value));
				}
				else {
					throw std::runtime_error("couldn't parse the attributes");
				}
			}
			// get the material
			if (primData.contains("mode")) {
				uint32_t mode = primData.at("mode");
				// TODO there's some way to do enum serialization with json but i don't care to learn it right now
				switch (mode) {
				case 0:
					builder.renderMode = Builder::RenderingMode::Points;
					break;
				case 1:
					builder.renderMode = Builder::RenderingMode::Lines;
					break;
				case 2:
					builder.renderMode = Builder::RenderingMode::Line_Loop;
					break;
				case 3:
					builder.renderMode = Builder::RenderingMode::Line_Strip;
					break;
				case 4:
					builder.renderMode = Builder::RenderingMode::Triangles;
					break;
				case 5:
					builder.renderMode = Builder::RenderingMode::Triangle_Strip;
					break;
				case 6:
					builder.renderMode = Builder::RenderingMode::Triangle_Fan;
					break;
				}
			}
			if (primData.contains("indices")) {
				builder.loadIndices();
			}
			builder.loadVertices();

			// using the accessor load the mesh primitive data

			// create the Mesh Primitive
			std::unique_ptr<MeshPrimitive> prim = std::make_unique<MeshPrimitive>(m_device, builder);
		}
	}

	void GlTFImporter::loadCamera(Camera& camera, json cameraData)
	{
	}

	void GlTFImporter::loadSkin(Skin& skin, json skinData)
	{
		// Skins are harder to implement, come back after you've tackled meshes
		if (!skinData.contains("joints"))
		{
			throw std::runtime_error("skin must contain an array of joints");
		}

	}
	void GlTFImporter::loadTexture(Texture& texture, json textureData)
	{

	}

	void GlTFImporter::loadMaterial(Material& material, json materialData)
	{
	}

	void GlTFImporter::loadSceneNode(SceneNode& sceneNode, json nodeData, uint32_t index)
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
			if (!isMeshDefined)
			{
				throw std::runtime_error("Mesh must be defined when skin is defined");
			}
			int skinIndex = nodeData["skin"];
			if (skinIndex > skins.size() - 1)
			{
				throw std::runtime_error("skin index out of range.");
			}
			else
			{
				Skin attachedSkin = *skins[skinIndex];
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
		if (nodeData.contains("weights"))
		{
			if (!isMeshDefined)
			{
				throw std::runtime_error("Mesh must be defined when skin is defined");
			}
			assert(nodeData.at("weights").size() == sceneNode.meshMorphTargetCount && "Mesh morph targets and weights are not the same size");
			// assign weights
		}
		if (nodeData.contains("children"))
		{
			// this is wrong, could potentially pull out children that haven't been initialized yet
			std::vector<uint32_t> nodeIndices = nodeData.at("children");

			nodeChildren.emplace(index, nodeIndices);
		}
	}

	void GlTFImporter::loadScene(Scene& scene, json sceneData)
	{
		// build the node hierarchy
		if (sceneData.contains("nodes")) {
			std::vector<uint32_t> nodeIndices = sceneData.at("nodes");
			for (uint32_t index : nodeIndices)
			{
				scene.children.push_back(sceneNodes.at(index));
			}
		}
		// add children to all scene nodes by accessing the nodeChildren adjacency list
		for (auto& [parentIndex, children] : nodeChildren) {
			for (uint32_t childIndex : children) {
				sceneNodes.at(parentIndex)->children.push_back(sceneNodes.at(childIndex));
			}
		}
	}
}
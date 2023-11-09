#include "gltf_importer.hpp"

#include "base64.hpp"

#include <memory>
#include <fstream>
#include <stdexcept>

namespace Ld {
	GlTFImporter::GlTFImporter(Device& device, const std::string& filepath, std::vector<Scene>& scenes)
		: m_device{ device }, m_filepath{filepath}
	{
		size_t trailingSlashPos = filepath.find_last_of("/");
		m_filepath = filepath.substr(0, trailingSlashPos + 1);
		std::ifstream f(filepath.c_str());
		json j;
		f >> j;


		if (!j.contains("asset"))
		{
			throw std::runtime_error("Asset is missing, gltf is invalid");
		}
		if (j.contains("buffers"))
		{
			for (json bufferData : j.at("buffers"))
			{
				GlTFBuffer buffer;
				loadBuffer(buffer, bufferData);
				buffers.push_back(buffer);
			}
		}
		if (j.contains("bufferViews"))
		{
			for (json bufferViewData : j.at("bufferViews"))
			{
				GlTFBufferView bufferView;
				loadBufferView(bufferView, bufferViewData);
				bufferViews.push_back(bufferView);
			}
		}
		if (j.contains("images")) 
		{
			for (json imageData : j.at("images"))
			{
				GlTFImage image;
				loadImage(image, imageData);
				images.push_back(image);
			}
		}
		if (j.contains("accessors"))  
		{
			for (json accessorData : j["accessors"])
			{
				GlTFAccessor accessor;
				loadAccessor(accessor, accessorData);
				accessors.push_back(accessor);
			}
		}
		if (j.contains("meshes"))
		{
			meshes.resize(j.at("meshes").size());
			std::vector<Mesh>::iterator it = meshes.begin();
			for (json meshData : j["meshes"])
			{
				Mesh mesh(m_device, meshData.at("primitives").size());
				loadMesh(mesh, meshData);
				try 
				{
					meshes.insert(++it, mesh);
				}
				catch (std::exception e)
				{
					throw e;
				}
			}
		}
		if (j.contains("cameras"))
		{
			for (json cameraData : j["cameras"])
			{
				Camera camera;
				loadCamera(camera, cameraData);
				cameras.push_back(camera);
			}
		if (j.contains("skins"))
		{
		}
			for (json skinData : j["skins"])
			{
				Skin skin;
				loadSkin(skin, skinData);
				skins.push_back(skin);
			}
		}
		if (j.contains("nodes"))
		{
			for (auto& [key, nodeData] : j.at("nodes").items())
			{
				SceneNode newNode;
				int index = 0;
				try {
					index = std::stoi(key);
				}
				catch (std::exception e) {
					throw e;
				}
				loadSceneNode(newNode, nodeData,index);
				sceneNodes.push_back(newNode);
			}
		}
		int defaultScene = 0;
		if (j.contains("scenes"))
		{
			size_t numScenes = j["scenes"].size();
			if (j.contains("scene")) {
				if (j["scene"] > numScenes - 1)
				{
					throw std::runtime_error("Default scene index exceeds number of scenes");
				}
			}
			defaultScene = j.at("scene");
			Scene newScene{};
			loadScene(newScene, j.at("scenes")[defaultScene]);
			scenes.push_back(newScene);
			if (numScenes > 1)
			{
				j.at("scenes").erase(defaultScene);
				for (json s : j["scenes"])
				{
					Scene newScene{};
					loadScene(newScene, s);
					scenes.push_back(newScene);
				}
			}

		}		
		// add children to all scene nodes by accessing the nodeChildren adjacency list
		for (auto& [parentIndex, children] : nodeChildren) {
			for (uint32_t childIndex : children) {
				sceneNodes.at(parentIndex).addChild(&sceneNodes.at(childIndex));
			}
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
		float i = 1.f;
		float* iPtr = &i;
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
					std::ifstream bin{ m_filepath + URI, std::ios::ate | std::ios::binary };

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
		uint32_t bufferIndex = bufferViewData.at("buffer");
		if (bufferIndex > buffers.size())
		{
			throw std::runtime_error("buffer index for this bufferview is out of range");
		}
		bufferView.bufferIndex = bufferIndex;
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
			image.bufferViewIndex = imageData.at("bufferView");
			image.mimeType = imageData.at("mimeType");
		}
		else {
			image.uri = imageData.at("uri");
			// TODO check if this is a data uri or a regular file
		}

	}

	void GlTFImporter::loadAccessor(GlTFAccessor& accessor, json accessorData)
	{
		if (!accessorData.contains("componentType") || !accessorData.contains("count") || !accessorData.contains("type"))
		{
			throw std::runtime_error("Accessor is missing required properties.");
		}

		accessor.count = accessorData.at("count");

		int componentType = accessorData.at("componentType");
		int byteSize = 0;
		switch (componentType)
		{
		case 5120: // byte
			accessor.componentType = GlTFAccessor::ComponentType::Byte;
			byteSize = 1;
			break;
		case 5121: // unsigned_byte
			accessor.componentType = GlTFAccessor::ComponentType::Unsigned_Byte;
			byteSize = 1;
			break;
		case 5122: // short
			accessor.componentType = GlTFAccessor::ComponentType::Short;
			byteSize = 2;
			break;
		case 5123: // unsigned_short
			accessor.componentType = GlTFAccessor::ComponentType::Unsigned_Short;
			byteSize = 2;
			break;
		case 5125: // unsigned_int
			accessor.componentType = GlTFAccessor::ComponentType::Unsigned_Int;
			byteSize = 4;
			break;
		case 5126: // float
			accessor.componentType = GlTFAccessor::ComponentType::Float;
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
			accessor.accessorType = GlTFAccessor::Type::Scalar;
			componentCount = 1;
		}
		else if (type == "VEC2")
		{
			accessor.accessorType = GlTFAccessor::Type::Vec2;
			componentCount = 2;
		}
		else if (type == "VEC3")
		{
			accessor.accessorType = GlTFAccessor::Type::Vec3;
			componentCount = 3;
		}
		else if (type == "VEC4")
		{
			accessor.accessorType = GlTFAccessor::Type::Vec4;
			componentCount = 4;
		}
		else if (type == "MAT2")
		{
			accessor.accessorType = GlTFAccessor::Type::Mat2;
			componentCount = 4;
		}
		else if (type == "MAT3")
		{
			accessor.accessorType = GlTFAccessor::Type::Mat3;
			componentCount = 9;
		}
		else if (type == "MAT4")
		{
			accessor.accessorType = GlTFAccessor::Type::Mat4;
			componentCount = 16;
		}
		else {
			throw std::runtime_error("Accessor Type not valid!");
		}
		accessor.maxes.resize(componentCount);
		accessor.mins.resize(componentCount);
		if (accessorData.contains("max"))  
		{
			for (int i = 0; i < componentCount; i++)
			{
				accessor.maxes[i] = accessorData.at("max")[i];
			}
		}
		if (accessorData.contains("min"))
		{
			for (int i = 0; i < componentCount; i++)
			{
				accessor.mins[i] = accessorData.at("min")[i];
			}
		}
		if (accessorData.contains("byteOffset"))
		{
			accessor.byteOffset = accessorData.at("byteOffset");
		}
		elementSize = byteSize * componentCount;
		if (accessorData.contains("sparse"))
		{
			// do sparse accessor functionality
		}
		// now get the buffer
		// this is technically the staging buffer. when the actual object (mesh or whatever) is created,
		// we copy this into the GPU
		GlTFBufferView* bufferView = nullptr;
		if (accessorData.contains("bufferView"))
		{
			accessor.bufferViewIndex = accessorData.at("bufferView");
		}

	}


	void GlTFImporter::loadMesh(Mesh& mesh, json meshData)
	{
		using Builder = MeshPrimitive::Builder;
		using AccessorMap = std::map<MeshPrimitive::Attribute, std::vector<uint8_t>>;
		for (json primData : meshData.at("primitives"))
		{
			if (!primData.contains("attributes"))
			{
				throw std::runtime_error("Primitive does not contain required attributes object");
			}
			// for each attribute
			// create primitive using a MeshPrimitive Builder
			MeshPrimitive::Builder builder{ };
			int ind = 0;
			for (auto& [key, value] : primData.at("attributes").items())
			{
				if (key.find("POSITION") == 0)
				{
					// value is the accessor that defines the vertices
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex); 
					auto bufferData = &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]);
					auto emplaceResult = builder.accessorData.emplace(MeshPrimitive::Attribute::Position, bufferData);
					if (!emplaceResult.second)
					{
						throw std::runtime_error("failed to emplace data");
					}
					builder.vertexCount = accessor.count;
					continue;
				}
				if (key.find("NORMAL") == 0)
				{
					// value is the accessor that defines the normals
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
					builder.accessorData.emplace(MeshPrimitive::Attribute::Normal, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
					continue;
				}
				if (key.find("TANGENT") == 0)
				{
					// value is the accessor that defines the tangents
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
					builder.accessorData.emplace(MeshPrimitive::Attribute::Tangent, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
					continue;
				}
				if (key.find("TEXCOORD_") == 0)
				{
					// value is the accessor that defines the Texcoords
					try
					{
						auto substr = key.substr(9);
						 ind = std::stoi(substr);
					}
					catch (std::exception e){
						throw e;
					}
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
					builder.accessorData.emplace(MeshPrimitive::Attribute::TexCoord, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
					continue;
				}
				if (key.find("COLOR_") == 0)
				{
					// value is the accessor that defines the color
					try
					{
						ind = std::stoi(key.substr(6));
					}
					catch (std::exception e) {
						throw e;
					}
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
					switch (accessor.accessorType)
					{
					case GlTFAccessor::Type::Vec3:
						builder.colorSize = 3;
						break;
					case GlTFAccessor::Type::Vec4:
						builder.colorSize = 4;
						break;
					default:
						throw std::runtime_error("Color size is invalid");
					}
					builder.accessorData.emplace(MeshPrimitive::Attribute::Color, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
					continue;
				}
				if (key.find("JOINTS_") == 0)
				{
					// value is the accessor that defines the joints
					try
					{
						ind = std::stoi(key.substr(7));
					}
					catch (std::exception e) {
						throw e;
					}
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
					builder.accessorData.emplace(MeshPrimitive::Attribute::Joints, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
					continue;
				}
				if (key.find("WEIGHTS_") == 0)
				{
					// value is the accessor that defines the weights
					try
					{
						ind = std::stoi(key.substr(8));
					}
					catch (std::exception e) {
						throw e;
					}
					const GlTFAccessor& accessor = accessors.at(value);
					const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
					builder.accessorData.emplace(MeshPrimitive::Attribute::Weights, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
					continue;
				}
				else {
					throw std::runtime_error("couldn't parse the attributes");
				}
				if (!builder.updateOrCheckCount(accessors.at(value).count))
				{
					throw std::runtime_error("some accessors have a different count");
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
				const GlTFAccessor& accessor = accessors.at(primData.at("indices"));
				builder.indexCount = accessor.count;
				const GlTFBufferView& view = bufferViews.at(accessor.bufferViewIndex);
				builder.accessorData.emplace(MeshPrimitive::Attribute::Indices, &(buffers.at(view.bufferIndex).data[accessor.byteOffset + view.byteOffset]));
			}
			// using the accessor load the mesh primitive data

			if (meshData.contains("name"))
			{
				mesh.name = meshData.at("name");
			}
			// create the Mesh Primitive
			mesh.addPrimitive(builder);
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
				sceneNode.mesh = &meshes[meshIndex];

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
				sceneNode.camera = &cameras[cameraIndex];
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
				Skin attachedSkin = skins[skinIndex];
				sceneNode.skin =&attachedSkin;
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
			std::vector<uint32_t> nodeIndices = nodeData.at("children");

			nodeChildren.emplace(index, nodeIndices);
		}
		if (nodeData.contains("name"))
		{
			sceneNode.name = nodeData.at("name");
		}
	}

	void GlTFImporter::loadScene(Scene& scene, json sceneData)
	{
		// build the node hierarchy
		if (sceneData.contains("nodes")) {
			std::vector<uint32_t> nodeIndices = sceneData.at("nodes");
			for (uint32_t index : nodeIndices)
			{
				scene.addNode(&sceneNodes.at(index));
			}
		}

	}
}
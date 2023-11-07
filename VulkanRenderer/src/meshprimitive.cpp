#include "meshprimitive.hpp"

namespace Ld {

	std::vector<VkVertexInputBindingDescription> MeshPrimitive::Vertex::getDefaultBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> MeshPrimitive::Vertex::getDefaultAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex,normal) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex,tangent) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex,texcoord) });
		attributeDescriptions.push_back({ 4, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex,color) });
		attributeDescriptions.push_back({ 5, 0, VK_FORMAT_R8G8B8A8_UINT, offsetof(Vertex, joints) });
		attributeDescriptions.push_back({ 6, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, weights) });

		return attributeDescriptions;
	}


	MeshPrimitive::Builder::Builder()
	{
	}

	bool MeshPrimitive::Builder::updateOrCheckCount(uint32_t count)
	{
		// update the attributecount if it is zero
		// otherwise check if it is the same as the attribute count
		// returns true if we update the count for the first time
		// or if the counts are already equal
		// otherwise return false
		if (attributeCount == 0 && count > 0)
		{
			attributeCount = count;
			return true;
		}
		else if (attributeCount == count)
		{
			return true;
		}
		return false;
	}

	const uint8_t* MeshPrimitive::Builder::getData(int accessorId) const
	{
		return (accessorData.count(accessorId) > 0) ? accessorData.at(accessorId).data() : nullptr;
	}


	MeshPrimitive::MeshPrimitive(Device& device, const MeshPrimitive::Builder& builder) : m_device{ device }
	{
		VkBufferCreateInfo bufferCreateInfo{};

		const float* positionBuffer = reinterpret_cast<const float*>(builder.getData(Builder::kPosition));
		const float* normalsBuffer = reinterpret_cast<const float*>(builder.getData(Builder::kNormal));
		const float* tangentsBuffer = reinterpret_cast<const float*>(builder.getData(Builder::kTangent));
		const float* texCoordsBuffer = reinterpret_cast<const float*>(builder.getData(Builder::kTexCoord));
		const float* colorBuffer = reinterpret_cast<const float*>(builder.getData(Builder::kColor));
		const unsigned short* jointsBuffer = reinterpret_cast<const unsigned short*>(builder.getData(Builder::kJoints));
		const float* weightsBuffer = reinterpret_cast<const float*>(builder.getData(Builder::kWeights));

		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		if (!builder.accessorData.empty())
		{
			m_vertexCount = builder.vertexCount;
			std::vector<uint8_t> data;

			for (size_t i = 0; i < m_vertexCount; i++)
			{
				Vertex vert{};
				if (positionBuffer != nullptr)
					vert.position = glm::vec3(positionBuffer[i * 3]);
				if (normalsBuffer != nullptr)
					vert.normal = glm::vec3(normalsBuffer[i * 3]);
				if (tangentsBuffer != nullptr)
					vert.tangent = glm::vec4(tangentsBuffer[i * 4]);
				if (texCoordsBuffer != nullptr)
					vert.texcoord = glm::vec2(texCoordsBuffer[i * 2]);
				if (colorBuffer != nullptr)
				{
					if (builder.colorSize == 3)
					{

						vert.color = glm::vec4(glm::vec3(colorBuffer[i * 3]), 1.f);
					}
					else
					{
						vert.color = glm::vec4(colorBuffer[i * 4]);
					}
				}
				if (jointsBuffer != nullptr)
					vert.joints = glm::vec4(jointsBuffer[i * 4]);
				if (weightsBuffer != nullptr)
					vert.weights = glm::vec4(weightsBuffer[i * 4]);

				vertices.push_back(vert);
			}
		}
		if (builder.indexCount > 0)
		{
			switch (builder.indexType) {
			case Builder::IndexType::unsigned_Int:
			{
				const uint32_t* indicesBuffer = reinterpret_cast<const uint32_t*>(builder.getData(Builder::kIndices));
				for (size_t i = 0; i < m_indexCount; i++) 
				{
					indices.push_back(indicesBuffer[i]);
				}
			}
			break;
			case Builder::IndexType::unsigned_Short:
			{
				const uint16_t* indicesBuffer = reinterpret_cast<const uint16_t*>(builder.getData(Builder::kIndices));
				for (size_t i = 0; i < m_indexCount; i++)
				{
					indices.push_back(indicesBuffer[i]);
				}
				break;
			}
			case Builder::IndexType::unsigned_Byte:
			{
				const uint8_t* indicesBuffer = reinterpret_cast<const uint8_t*>(builder.getData(Builder::kIndices));
				for (size_t i = 0; i < m_indexCount; i++)
				{
					indices.push_back(indicesBuffer[i]);
				}
				break;
			}
			default:
				throw std::runtime_error("index type not supported");
			}
			m_indexCount = indices.size();
		}
	}

	MeshPrimitive::~MeshPrimitive()
	{
	}



	void MeshPrimitive::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_vertexBuffer->getBuffer() };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets.data());
		if (m_hasIndexBuffer)
		{
			// optimize for smaller models?
			vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void MeshPrimitive::draw(VkCommandBuffer commandBuffer)
	{
		if (m_hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
		}
	}

	void MeshPrimitive::createVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);


		VkBufferCreateInfo stagingBufferCreateInfo{};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.size = bufferSize;
		stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingAllocCreateInfo{};
		stagingAllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		stagingAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		stagingAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		//uint32_t alignmentSize = Ld::Buffer::getAlignment(vertexSize, 1);
		Buffer stagingBuffer{
			m_device,
			stagingBufferCreateInfo,
			stagingAllocCreateInfo,
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		VkBufferCreateInfo vertexBufferCreateInfo{};
		vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferCreateInfo.size = bufferSize;
		vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo vertexAllocCreateInfo{};
		vertexAllocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		vertexAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		vertexAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;


		m_vertexBuffer = std::make_unique<Buffer>(
			m_device,
			vertexBufferCreateInfo,
			vertexAllocCreateInfo
		);
		m_device.copyBuffer(stagingBuffer.getBuffer(), m_vertexBuffer->getBuffer(), bufferSize);
	}

	void MeshPrimitive::createIndexBuffers(const std::vector<uint32_t>& indices)
	{
		m_indexCount = static_cast<uint32_t>(indices.size());
		m_hasIndexBuffer = m_indexCount > 0;

		if (!m_hasIndexBuffer)
		{
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_indexCount;
		uint32_t indexSize = sizeof(indices[0]);


		VkBufferCreateInfo stagingBufferCreateInfo{};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.size = bufferSize;
		stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo stagingAllocInfo{};
		stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		//	uint32_t alignmentSize = Ld::Buffer::getAlignment(indexSize, 1);
		Buffer stagingBuffer{
			m_device,
			stagingBufferCreateInfo,
			stagingAllocInfo,
		};
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		VkBufferCreateInfo indexBufferCreateInfo{};
		indexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		indexBufferCreateInfo.size = bufferSize;
		indexBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		VmaAllocationCreateInfo indexAllocInfo{};
		indexAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		indexAllocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		indexAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

		m_indexBuffer = std::make_unique<Buffer>(
			m_device,
			indexBufferCreateInfo,
			indexAllocInfo
		);

		m_device.copyBuffer(stagingBuffer.getBuffer(), m_indexBuffer->getBuffer(), bufferSize);
	}
}
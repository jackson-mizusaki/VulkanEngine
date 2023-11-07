#include "accessor.hpp"

namespace Ld {
	Accessor::Accessor(Device& device) : m_device{ device }
	{
		VkBufferCreateInfo bufferCreateInfo{};
	}

	void Accessor::loadBuffer(const std::vector<uint8_t>& data, VkDeviceSize size)
	{
		VkBufferCreateInfo stagingBufferCreateInfo{};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.size = size;
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
		buffer = &stagingBuffer;
		buffer->map();
		buffer->writeToBuffer((void*)data.data());
		buffer->unmap();

	}


}

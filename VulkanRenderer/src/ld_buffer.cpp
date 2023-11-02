/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "ld_buffer.hpp"

 // std
#include <cassert>
#include <cstring>
#include <stdexcept>

namespace Ld {
	//Buffer::Buffer(Device& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
	//	: m_device{ device }, m_instanceSize{ instanceSize }, m_instanceCount{ instanceCount }, m_usageFlags{ usageFlags }, m_memoryPropertyFlags{ memoryPropertyFlags }
	//{
	//	m_alignmentSize = getAlignment(instanceSize, minOffsetAlignment);
	//	m_bufferSize = m_alignmentSize * instanceCount;
	//	device.createBuffer(m_bufferSize, usageFlags, memoryPropertyFlags, m_buffer, m_allocation);
	//}

	Buffer::Buffer(Device& device, VkBufferCreateInfo& createInfo, VmaAllocationCreateInfo& allocCreateInfo)
		: m_device{ device }//, m_instanceSize{ instanceSize }, m_alignmentSize { alignmentSize }
	{	 
		//m_bufferSize = m_alignmentSize * instanceCount;
		//createInfo.size = m_bufferSize;
		createBuffer(createInfo, allocCreateInfo);
	}

	Buffer::~Buffer()
	{
		unmap();
		vmaDestroyBuffer(m_device.getAllocator(), m_buffer, m_allocation);
		//vkDestroyBuffer(m_device.device(), m_buffer, nullptr);
		//vkFreeMemory(m_device.device(), m_memory, nullptr);
	}

	/**
	 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
	 *
	 * @param instanceSize The size of an instance
	 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
	 * minUniformBufferOffsetAlignment)
	 *
	 * @return VkResult of the buffer mapping call
	 */
	VkDeviceSize Buffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
		if (minOffsetAlignment > 0) {
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	void Buffer::createBuffer(VkBufferCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo)
	{
		assert(m_device.hasAllocator() && "allocator for device not created!");

		VkResult result = vmaCreateBuffer(m_device.getAllocator(), &createInfo, &allocInfo, &m_buffer, &m_allocation, &m_allocationInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}
	}

	/**
	 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	 *
	 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
	 * buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the buffer mapping call
	 */
	VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset)
	{
		assert(m_buffer && m_allocation && "Called map on buffer before create");
		return vmaMapMemory(m_device.getAllocator(), m_allocation, &m_mapped);
		//return vkMapMemory(m_device.device(), m_memory, offset, size, 0, &m_mapped);
	}

	/**
	 * Unmap a mapped memory range
	 *
	 * @note Does not return a result as vkUnmapMemory can't fail
	 */
	void Buffer::unmap()
	{
		if (m_mapped)
		{
			vmaUnmapMemory(m_device.getAllocator(), m_allocation);
			//vkUnmapMemory(m_device.device(), m_memory);
			m_mapped = nullptr;
		}
	}

	/**
	 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
	 *
	 * @param data Pointer to the data to copy
	 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
	 * range.
	 * @param offset (Optional) Byte offset from beginning of mapped region
	 *
	 */
	void Buffer::writeToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		assert(m_mapped && "Cannot copy to unmapped buffer");

		if (size == VK_WHOLE_SIZE)
		{
			memcpy(m_mapped, data, m_allocationInfo.size);
		}
		else
		{
			char* memOffset = (char*)m_mapped;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	/**
	 * Flush a memory range of the buffer to make it visible to the device
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
	 * complete buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the flush call
	 */
	VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		return vmaFlushAllocation(m_device.getAllocator(), m_allocation, offset, size);
		//VkMappedMemoryRange mappedRange = {};
		//mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		//mappedRange.memory = m_memory;
		//mappedRange.offset = offset;
		//mappedRange.size = size;
		//return vkFlushMappedMemoryRanges(m_device.device(), 1, &mappedRange);
	}

	/**
	 * Invalidate a memory range of the buffer to make it visible to the host
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
	 * the complete buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the invalidate call
	 */
	VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		return vmaInvalidateAllocation(m_device.getAllocator(), m_allocation, offset, size);
		//VkMappedMemoryRange mappedRange = {};
		//mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		//mappedRange.memory = m_memory;
		//mappedRange.offset = offset;
		//mappedRange.size = size;
		//return vkInvalidateMappedMemoryRanges(m_device.device(), 1, &mappedRange);
	}

	/**
	 * Create a buffer info descriptor
	 *
	 * @param size (Optional) Size of the memory range of the descriptor
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkDescriptorBufferInfo of specified offset and range
	 */
	VkDescriptorBufferInfo Buffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset)
	{
		return VkDescriptorBufferInfo{
			m_buffer,
			offset,
			size,
		};
	}
}  // namespace Ld

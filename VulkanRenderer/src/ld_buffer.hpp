#pragma once

#include "ld_device.hpp" 
#include "ld_allocator.hpp"

namespace Ld {
	class Buffer {
	public: // constructors
/*		Buffer(
			Device& device,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryPropertyFlags,
			VkDeviceSize minOffsetAlignment = 1);	*/	
		Buffer(
				Device& device,
				VkBufferCreateInfo& createInfo,
				VmaAllocationCreateInfo& allocInfo
		);
		~Buffer();

		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

	public: // functions
		VkBuffer getBuffer() const { return m_buffer; }
		void* getMappedMemory() const { return m_allocationInfo.pMappedData; }
		//uint32_t getInstanceCount() const { return m_instanceCount; }
		//VkDeviceSize getInstanceSize() const { return m_instanceSize; }
		//VkDeviceSize getAlignmentSize() const { return m_alignmentSize; }
		//VkBufferUsageFlags getUsageFlags() const { return m_usageFlags; }
		//VkMemoryPropertyFlags getMemoryPropertyFlags() const { return m_memoryPropertyFlags; }
		VkDeviceSize getBufferSize() const { return m_allocationInfo.size; }
		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		void unmap();

		void writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
		VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);
	private:
		void createBuffer(VkBufferCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo);

	public: // data
	private:
		Device& m_device;
		void* m_mapped = nullptr;
		VkBuffer m_buffer = VK_NULL_HANDLE;
		VmaAllocation m_allocation;
		VmaAllocationInfo m_allocationInfo{};

		//VkDeviceSize m_bufferSize;
		//uint32_t m_instanceCount;
		//VkDeviceSize m_instanceSize;
		//VkDeviceSize m_alignmentSize;
		//VkBufferUsageFlags m_usageFlags;
	//	VkMemoryPropertyFlags m_memoryPropertyFlags;

	};

}  // namespace Ld
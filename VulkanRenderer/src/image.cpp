#include "image.hpp"
#include <cassert>
#include <stdexcept>
namespace Ld {
	Image::Image(Device& device, VkImageCreateInfo& createInfo, VkDeviceSize instanceSize, uint32_t instanceCount, VmaAllocationCreateInfo& allocInfo, VkDeviceSize alignmentSize)
		: m_device{ device }, m_instanceSize{ instanceSize }, m_alignmentSize{ alignmentSize }
	{

	}
	void Image::createImage(VkImageCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo)
	{
		assert(m_device.hasAllocator() && "allocator for device not created!");

		if (vmaCreateImage(m_device.getAllocator(), &createInfo, &allocInfo, &m_image, &m_allocation, &m_allocationInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}
	}
}
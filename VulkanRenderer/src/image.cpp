#include "image.hpp"
#include <cassert>
#include <stdexcept>
namespace Ld {
	Image::Image(Device& device, VkImageCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo)
		: m_device{ device }
	{
		m_extent = createInfo.extent;
		createImage(createInfo, allocInfo);
	}

	Image::~Image()
	{
		
		vmaDestroyImage(m_device.getAllocator(), m_image, m_allocation);
	}

	void Image::createImage(VkImageCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo)
	{
		assert(m_device.hasAllocator() && "allocator for device not created!");

		VkResult result = vmaCreateImage(m_device.getAllocator(), &createInfo, &allocInfo, &m_image, &m_allocation, &m_allocationInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}
	}

	void Image::transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		m_device.transitionImageLayout(m_image, oldLayout, newLayout);
	}
	void Image::copyBufferToImage(Buffer& buffer)
	{
		m_device.copyBufferToImage(buffer.getBuffer(), m_image, m_extent.width, m_extent.height, m_layerCount);
	}
}
#pragma once

#include "ld_device.hpp" 
#include "ld_allocator.hpp"
#include "ld_buffer.hpp"
#include <memory>
namespace Ld {
	class Image {
	public: // constructors
		Image(
			Device& device,
			VkImageCreateInfo& createInfo,
			VmaAllocationCreateInfo& allocInfo
		);

		~Image();               // destructor 
		Image(const Image&) = default;                // copy constructor
		Image& operator=(const Image&) = default;     // copy assignment
		Image(Image&&) noexcept = default;            // move constructor
		Image& operator=(Image&&) noexcept = default; // move assignment

	public:
		VkImage getImage() { return m_image; }
		VkImageView getImageView() { return m_imageView; }	
		void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
		void copyBufferToImage(Buffer& buffer);
	private:
		void createImage(VkImageCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo);
		//static std::unique_ptr<Image> loadImageFromFile(Device& device, const std::string& filepath);
		void createImageView();
		void loadImage(const std::string& filepath);

	public: // data
	private:
		Device& m_device;
		void* m_mapped = nullptr;
		VkImage m_image = VK_NULL_HANDLE;
		VkImageView m_imageView = VK_NULL_HANDLE;
		VmaAllocation m_allocation;
		VmaAllocationInfo m_allocationInfo{};
		VkExtent3D m_extent;
		uint32_t m_layerCount{ 1 };
		VkDeviceSize m_instanceSize;
		VkDeviceSize m_alignmentSize;

	};
}
#pragma once

#include "ld_device.hpp" 
#include "ld_allocator.hpp"
#include <memory>
namespace Ld {
	class Image {
	public: // constructors
		Image(
			Device& device,
			VkImageCreateInfo& createInfo,
			VkDeviceSize instanceSize,
			uint32_t instanceCount,
			VmaAllocationCreateInfo& allocInfo,
			VkDeviceSize alignmentSize);

		~Image();               // destructor 
		Image(const Image&) = default;                // copy constructor
		Image& operator=(const Image&) = default;     // copy assignment
		Image(Image&&) noexcept = default;            // move constructor
		Image& operator=(Image&&) noexcept = default; // move assignment

	public:
	private:
		void createImage(VkImageCreateInfo& createInfo, VmaAllocationCreateInfo& allocInfo);
		static std::unique_ptr<Image> loadImageFromFile(Device& device, const std::string& filepath);
		void loadImage(const std::string& filepath);

	public: // data
	private:
		Device& m_device;
		void* m_mapped = nullptr;
		VkImage m_image = VK_NULL_HANDLE;
		VmaAllocation m_allocation;
		VmaAllocationInfo m_allocationInfo;

		VkDeviceSize m_instanceSize;
		VkDeviceSize m_alignmentSize;

	};
}
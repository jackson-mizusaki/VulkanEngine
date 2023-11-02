#pragma once
#include "ld_device.hpp"

#include "image.hpp"
#include <vulkan/vulkan.h>

#include <string>
#include <memory>

namespace Ld {
	class Texture {
	public: // constructors
		Texture(Device& device, const std::string& filepath);
		~Texture();               // destructor 
		Texture(const Texture&) = default;                // copy constructor
		Texture& operator=(const Texture&) = default;     // copy assignment
		Texture(Texture&&) noexcept = default;            // move constructor
		Texture& operator=(Texture&&) noexcept = default; // move assignment

	public: // functions 
		static std::unique_ptr<Texture> createTextureFromFile(Device& device, const std::string& filepath);
		void createTextureImage(const std::string& filepath);
		void createTextureSampler();
	private:
		void initializeImage();
		void createTextureImageView(VkImageViewType viewType);

	private: // data
		Device& m_device;
		std::unique_ptr<Image> m_image = nullptr; // image may be shared by many objects
		VmaAllocation m_allocation;
		VmaAllocationInfo m_allocationInfo;

		VkSampler m_textureSampler = nullptr;
		VkFormat m_format;
		VkImageLayout m_textureLayout;
		uint32_t m_mipLevels{ 1 };
		uint32_t m_layerCount{ 1 };
		VkExtent3D m_extent;
	};
}
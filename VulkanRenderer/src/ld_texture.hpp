#pragma once
#include "ld_device.hpp"

#include "image.hpp"
#include <vulkan/vulkan.h>

#include <string>
#include <memory>

namespace Ld {
	class Texture {
		friend class Image;
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
	private:
		void initializeImage();

	private: // data
		Device& m_device;
		std::shared_ptr<Image> m_image = nullptr;
		VkImage m_textureImage = nullptr;
		VmaAllocation m_allocation;
		VkImageView m_textureImageView = nullptr;
		VkSampler m_textureSampler = nullptr;
		VkFormat m_format;
		VkImageLayout m_textureLayout;
		uint32_t m_mipLevels{ 1 };
		uint32_t m_layerCount{ 1 };
		VkExtent3D m_extent{};
	};
}
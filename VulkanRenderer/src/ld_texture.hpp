#pragma once
#include "ld_device.hpp"
#include <vulkan/vulkan.h>

#include <string>
#include <memory>

namespace Ld {
	class LdTexture {
	public: // constructors
		LdTexture(LdDevice& device, const std::string& filepath);
		~LdTexture();               // destructor 
		LdTexture(const LdTexture&) = default;                // copy constructor
		LdTexture& operator=(const LdTexture&) = default;     // copy assignment
		LdTexture(LdTexture&&) noexcept = default;            // move constructor
		LdTexture& operator=(LdTexture&&) noexcept = default; // move assignment

	public: // functions 
		static std::unique_ptr<LdTexture> createTextureFromFile(LdDevice& device, const std::string& filepath);
		void createTextureImage(const std::string& filepath);

	private: // data
		LdDevice& m_device;
		VkImage m_textureImage = nullptr;
		VkDeviceMemory m_textureImageMemory = nullptr;
		VkImageView m_textureImageView = nullptr;
		VkSampler m_textureSampler = nullptr;
		VkFormat m_format;
		VkImageLayout m_textureLayout;
		uint32_t m_mipLevels{ 1 };
		uint32_t m_layerCount{ 1 };
		VkExtent3D m_extent{};
	};
}
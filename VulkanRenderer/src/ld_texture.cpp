#include "ld_texture.hpp"

#include "ld_buffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace Ld {
	Texture::Texture(Device& device, const std::string& filepath)
		: m_device{device}
	{
		createTextureImage(filepath);
	}

	Texture::~Texture()
	{
		vkDestroySampler(m_device.device(), m_textureSampler, nullptr);
		vmaDestroyImage(m_device.getAllocator(), m_textureImage, m_allocation);
	}

	std::unique_ptr<Texture> Texture::createTextureFromFile(Device& device, const std::string& filepath)
	{
		return std::make_unique<Texture>(device, filepath);
	}

	void Texture::createTextureImage(const std::string& filepath)
	{
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load(filepath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		VmaAllocationCreateInfo stagingInfo{};
		stagingInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		Buffer stagingBuffer{
			m_device,
			imageSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			stagingInfo
		};
		 
		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)pixels, imageSize);

		stagingBuffer.unmap();
		stbi_image_free(pixels);

		m_mipLevels = 1; // extend for mipmaps

		m_format = VK_FORMAT_R8G8B8A8_SRGB;
		m_extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent = m_extent;
		imageInfo.mipLevels = m_mipLevels;
		imageInfo.arrayLayers = m_layerCount;
		imageInfo.format = m_format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo imageAllocInfo{};
		imageAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		m_device.createImageWithInfo(
			imageInfo,
			m_textureImage,
			m_allocation,
			imageAllocInfo			
		);
		m_device.transitionImageLayout(
			m_textureImage,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);
		m_device.copyBufferToImage(
			stagingBuffer.getBuffer(),
			m_textureImage, 
			static_cast<uint32_t>(texWidth),
			static_cast<uint32_t>(texHeight),
			m_layerCount
		);

		m_device.transitionImageLayout(
			m_textureImage, 
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);
	}
}
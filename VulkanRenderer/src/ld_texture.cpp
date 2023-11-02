#include "ld_texture.hpp"

#include "ld_buffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace Ld {
	Texture::Texture(Device& device, const std::string& filepath)
		: m_device{ device }
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
		m_format = VK_FORMAT_R8G8B8A8_SRGB;
		m_extent = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1};
		m_mipLevels = 1;

		initializeImage();

		assert(m_image && "Image was not initialized");
		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}


		VkBufferCreateInfo stagingBufferCreateInfo{};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.size = imageSize;
		stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocCreateInfo{};
		allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

		Buffer stagingBuffer(
			m_device,
			stagingBufferCreateInfo,
			allocCreateInfo
		);

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(pixels);
		stagingBuffer.unmap();

		stbi_image_free(pixels);


		m_image->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		m_image->copyBufferToImage(
			stagingBuffer);

		// comment this out if using mips
		m_image->transitionImageLayout(	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
		// mDevice.generateMipmaps(mTextureImage, mFormat, texWidth, texHeight, mMipLevels);
		m_textureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		
	}
	void Texture::initializeImage()
	{
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

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
		//allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;

		// create image
		m_image = std::make_shared<Image>(
			m_device,
			imageInfo,
			allocInfo
		);


	}
}
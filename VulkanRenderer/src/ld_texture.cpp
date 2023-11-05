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
		m_extent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };
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
		m_image->transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		// If we generate mip maps then the final image will alerady be READ_ONLY_OPTIMAL
		// mDevice.generateMipmaps(mTextureImage, mFormat, texWidth, texHeight, mMipLevels);
		m_textureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	}

	void Texture::createTextureImage(Image* image)
	{
	}

	void Texture::createTextureSampler(const Sampler& sampler)
	{
		VkSamplerCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		switch (sampler.magFilter)
		{
		case Sampler::MagnificationFilter::Nearest:
			createInfo.magFilter = VK_FILTER_NEAREST;
			break;
		case Sampler::MagnificationFilter::Linear:
			createInfo.magFilter = VK_FILTER_LINEAR;
			break;
		default:
			// error?
			break;
		}
		switch (sampler.minFilter)
		{
		case Sampler::MinificationFilter::Nearest:
			createInfo.minFilter = VK_FILTER_NEAREST;
			break;
		case Sampler::MinificationFilter::Linear:
			createInfo.minFilter = VK_FILTER_LINEAR;
			break;
		case Sampler::MinificationFilter::NearestMipMapNearest:
			// generate MipMaps?  or fallback to VK_FILTER_NEAREST
			createInfo.minFilter = VK_FILTER_NEAREST;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			break;
		case Sampler::MinificationFilter::LinearMipMapNearest:
			// generate MipMaps?
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			break;
		case Sampler::MinificationFilter::NearestMipMapLinear:
			// generate MipMaps?  or fallback to VK_FILTER_NEAREST
			createInfo.minFilter = VK_FILTER_NEAREST;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			break;
		case Sampler::MinificationFilter::LinearMipMapLinear:
			// generate MipMaps?
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			break;
		default:
			// error?
			break;
		}
		switch (sampler.wrapS)
		{
		case Sampler::WrapMode::ClampToEdge:
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			break;
		case Sampler::WrapMode::MirroredRepeat:
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			break;
		case Sampler::WrapMode::Repeat:
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			break;
		default:
			// error?
			break;
		}
		switch (sampler.wrapT)
		{
		case Sampler::WrapMode::ClampToEdge:
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			break;
		case Sampler::WrapMode::MirroredRepeat:
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			break;
		case Sampler::WrapMode::Repeat:
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			break;
		default:
			// error?
			break;
		}

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(m_device.getPhysicalDevice(), &properties);
		createInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		createInfo.unnormalizedCoordinates = VK_FALSE;

		createInfo.compareEnable = VK_FALSE;
		createInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		createInfo.mipLodBias = 0.0f;
		createInfo.minLod = 0.0f;
		createInfo.maxLod = 0.0f;
		if (vkCreateSampler(m_device.device(), &createInfo, nullptr, &m_textureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
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
		m_image = std::make_unique<Image>(
			m_device,
			imageInfo,
			allocInfo
		);


	}
	void Texture::createTextureImageView(VkImageViewType viewType)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.viewType = viewType;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = m_mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = m_layerCount;
		if (m_image->createImageView(viewInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view!");
		}
	}

	//void Texture::createTextureSampler()
	//{
	//	VkSamplerCreateInfo createInfo{};
	//	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	//	createInfo.magFilter = VK_FILTER_LINEAR;
	//	createInfo.minFilter = VK_FILTER_LINEAR;

	//	createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//	createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//	createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	//	createInfo.anisotropyEnable = VK_TRUE;
	//	createInfo.maxAnisotropy = 16.0f;
	//	createInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	//	createInfo.unnormalizedCoordinates = VK_FALSE;

	//	// these fields useful for percentage close filtering for shadow maps
	//	createInfo.compareEnable = VK_FALSE;
	//	createInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	//	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	//	createInfo.mipLodBias = 0.0f;
	//	createInfo.minLod = 0.0f;
	//	createInfo.maxLod = static_cast<float>(m_mipLevels);

	//	if (vkCreateSampler(m_device.device(), &createInfo, nullptr, &m_textureSampler) != VK_SUCCESS) {
	//		throw std::runtime_error("failed to create texture sampler!");
	//	}
	//}
}
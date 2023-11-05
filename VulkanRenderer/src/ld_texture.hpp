#pragma once
#include "ld_device.hpp"

#include "image.hpp"
#include <vulkan/vulkan.h>

#include <string>
#include <memory>

namespace Ld {
	class Texture {
	public: // Types
		class Sampler {
		public: // types
			enum MagnificationFilter {
				Nearest,
				Linear
			};
			enum MinificationFilter {
				Nearest,
				Linear,
				NearestMipMapNearest,
				LinearMipMapNearest,
				NearestMipMapLinear,
				LinearMipMapLinear
			};
			enum WrapMode {
				ClampToEdge,
				MirroredRepeat,
				Repeat
			};
		public: // data
			MagnificationFilter magFilter;
			MinificationFilter minFilter;
			WrapMode wrapS = Repeat;
			WrapMode wrapT = Repeat;
			std::string name;
			//json extensions
			//json extras
		};
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
		void createTextureImage(Image* image);
		void createTextureSampler(const Sampler& sampler);
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
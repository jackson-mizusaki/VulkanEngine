#pragma once

#include "ld_window.hpp"
#include "vk_mem_alloc.h"
// std lib headers
#include <string>
#include <vector>

namespace Ld {
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		uint32_t graphicsFamily;
		uint32_t presentFamily;
		bool graphicsFamilyHasValue = false;
		bool presentFamilyHasValue = false;
		bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class Device {
		friend class Allocator;
	public: // constructors
		Device(Window& window);
		~Device();
		// Not copyable or movable
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

	public: // functions
		VkCommandPool getCommandPool() { return m_commandPool; }
		VkDevice device() { return m_device; }
		VkSurfaceKHR surface() { return m_surface; }
		VkQueue graphicsQueue() { return m_graphicsQueue; }
		VkQueue presentQueue() { return m_presentQueue; }

		SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(m_physicalDevice); }
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(m_physicalDevice); }
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VmaAllocator getAllocator() { return m_allocator;  }

		// Buffer Helper Functions
		void createBuffer(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkBuffer& buffer,
			VmaAllocation& allocation,
			VmaAllocationCreateInfo& allocInfo);

		void createImageWithInfo(const VkImageCreateInfo& imageInfo,
			VkImage& image,
			VmaAllocation& allocation,
			VmaAllocationCreateInfo& allocInfo);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
		void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);


	private:
		void createAllocator();
		void createInstance();
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createCommandPool();

		
		// helper functions
		bool isDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void hasGflwRequiredInstanceExtensions();
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	public: // data
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
		VkPhysicalDeviceProperties properties;
	private:
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		Window& m_window;
		VkCommandPool m_commandPool;

		VkDevice m_device;
		VkSurfaceKHR m_surface;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;

		VmaAllocator m_allocator;

		//Allocator m_allocator{ m_device };


		const std::vector<const char*> m_validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};
}// namespace lve
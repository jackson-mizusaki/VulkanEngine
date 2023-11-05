#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "ld_allocator.hpp"

namespace Ld {
	Allocator::Allocator(VkDevice device)
	{
		VmaAllocatorCreateInfo allocatorCreateInfo = {};
	//	defaultVmaAllocatorCreateInfo(device, allocatorCreateInfo);
		createAllocator(allocatorCreateInfo, &m_allocator);
	}


	void Allocator::createAllocator(VmaAllocatorCreateInfo& createInfo, VmaAllocator* allocator)
	{
		vmaCreateAllocator(&createInfo, allocator);
	}
}
#pragma once

#include "ld_device.hpp"
#include "vk_mem_alloc.h"

namespace Ld {
	class Allocator {
	public: // constructors
		Allocator(VkDevice device);
		~Allocator() = default;               // destructor 
		Allocator(const Allocator&) = default;                // copy constructor
		Allocator& operator=(const Allocator&) = default;     // copy assignment
		Allocator(Allocator&&) noexcept = default;            // move constructor
		Allocator& operator=(Allocator&&) noexcept = default; // move assignment

	public: // functions
		void createAllocator(VmaAllocatorCreateInfo& createInfo, VmaAllocator* allocator);
		VmaAllocator getAllocator() { return m_allocator; }

	private: // data
		VmaAllocator m_allocator{};
	};
}
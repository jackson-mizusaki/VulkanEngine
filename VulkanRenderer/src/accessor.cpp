#include "accessor.hpp"

namespace Ld {
	Accessor::Accessor(Device& device) : m_device{ device }
	{
		VkBufferCreateInfo bufferCreateInfo{};
	}

	void Accessor::loadBuffer()
	{

		VkBufferViewCreateInfo bufferViewCreateInfo{};
		bufferViewCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;

	}

}

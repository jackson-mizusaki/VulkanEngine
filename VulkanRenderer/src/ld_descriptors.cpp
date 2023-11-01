#include "ld_descriptors.hpp"

// std
#include <cassert>
#include <stdexcept>

namespace Ld {

	// *************** Descriptor Set Layout Builder *********************

	LdDescriptorSetLayout::Builder& LdDescriptorSetLayout::Builder::addBinding(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count)
	{
		assert(bindings.count(binding) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<LdDescriptorSetLayout> LdDescriptorSetLayout::Builder::build() const
	{
		return std::make_unique<LdDescriptorSetLayout>(ldDevice, bindings);
	}

	// *************** Descriptor Set Layout *********************

	LdDescriptorSetLayout::LdDescriptorSetLayout(LdDevice& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: m_device{ device }, m_bindings{ bindings }, m_descriptorSetLayout{}
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto& kv : bindings)
		{
			setLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_device.device(), &descriptorSetLayoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	LdDescriptorSetLayout::~LdDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_device.device(), m_descriptorSetLayout, nullptr);
	}

	// *************** Descriptor Pool Builder *********************

	LdDescriptorPool::Builder& LdDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
	{
		poolSizes.push_back({ descriptorType, count });
		return *this;
	}

	LdDescriptorPool::Builder& LdDescriptorPool::Builder::setPoolFlags(VkDescriptorPoolCreateFlags flags)
	{
		poolFlags = flags;
		return *this;
	}
	LdDescriptorPool::Builder& LdDescriptorPool::Builder::setMaxSets(uint32_t count)
	{
		maxSets = count;
		return *this;
	}

	std::unique_ptr<LdDescriptorPool> LdDescriptorPool::Builder::build() const
	{
		return std::make_unique<LdDescriptorPool>(ldDevice, maxSets, poolFlags, poolSizes);
	}

	// *************** Descriptor Pool *********************

	LdDescriptorPool::LdDescriptorPool(LdDevice& device,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes)
		: m_device{ device }
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(m_device.device(), &descriptorPoolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	LdDescriptorPool::~LdDescriptorPool()
	{
		vkDestroyDescriptorPool(m_device.device(), m_descriptorPool, nullptr);
	}

	bool LdDescriptorPool::allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(m_device.device(), &allocInfo, &descriptor) != VK_SUCCESS)
		{
			return false;
		}
		return true;
	}

	void LdDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets(m_device.device(), m_descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
	}

	void LdDescriptorPool::resetPool()
	{
		vkResetDescriptorPool(m_device.device(), m_descriptorPool, 0);
	}

	// *************** Descriptor Writer *********************

	LdDescriptorWriter::LdDescriptorWriter(LdDescriptorSetLayout& setLayout, LdDescriptorPool& pool)
		: m_setLayout{ setLayout }, m_pool{ pool }
	{
	}

	LdDescriptorWriter& LdDescriptorWriter::writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = m_setLayout.m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		m_writes.push_back(write);
		return *this;
	}

	LdDescriptorWriter& LdDescriptorWriter::writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		assert(m_setLayout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = m_setLayout.m_bindings[binding];

		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		m_writes.push_back(write);
		return *this;
	}

	bool LdDescriptorWriter::build(VkDescriptorSet& set)
	{
		bool success = m_pool.allocateDescriptorSet(m_setLayout.getDescriptorSetLayout(), set);
		if (!success)
		{
			return false;
		}
		overwrite(set);
		return true;
	}

	void LdDescriptorWriter::overwrite(VkDescriptorSet& set)
	{
		for (auto& write : m_writes)
		{
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(m_pool.m_device.device(), static_cast<uint32_t>(m_writes.size()), m_writes.data(), 0, nullptr);
	}

}  // namespace lve

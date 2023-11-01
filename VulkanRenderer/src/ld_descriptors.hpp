#pragma once

#include "ld_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Ld {
	class DescriptorSetLayout {
		friend class DescriptorWriter;

	public: // types
		class Builder {
		public:
			Builder(Device& device) : ldDevice{ device } {}

		public:
			Builder& addBinding(uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);

			std::unique_ptr<DescriptorSetLayout> build() const;

		private:
			Device& ldDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

	public: // constructors
		DescriptorSetLayout(Device& ldDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~DescriptorSetLayout();
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;

	public: // functions
		VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

	public: // data
	private:
		Device& m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;
	};

	class DescriptorPool {
		friend class DescriptorWriter;

	public: // types
		class Builder {
		public:
			Builder(Device& ldDevice) : ldDevice{ ldDevice } {}

		private:
			Device& ldDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;

		public:
			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<DescriptorPool> build() const;
		};

	public: // constructors
		DescriptorPool(Device& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~DescriptorPool();
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;

	public: // functions
		bool allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
		void resetPool();

	private: // data
		Device& m_device;
		VkDescriptorPool m_descriptorPool;
	};

	class DescriptorWriter {
	public: // constructors
		DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

	public: // functions
		DescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		DescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private: // data
		DescriptorSetLayout& m_setLayout;
		DescriptorPool& m_pool;
		std::vector<VkWriteDescriptorSet> m_writes;
	};

}  // namespace lve
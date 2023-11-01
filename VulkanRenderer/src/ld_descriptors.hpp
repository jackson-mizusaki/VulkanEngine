#pragma once

#include "ld_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace Ld {
	class LdDescriptorSetLayout {
		friend class LdDescriptorWriter;

	public: // types
		class Builder {
		public:
			Builder(LdDevice& device) : ldDevice{ device } {}

		public:
			Builder& addBinding(uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);

			std::unique_ptr<LdDescriptorSetLayout> build() const;

		private:
			LdDevice& ldDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

	public: // constructors
		LdDescriptorSetLayout(LdDevice& ldDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~LdDescriptorSetLayout();
		LdDescriptorSetLayout(const LdDescriptorSetLayout&) = delete;
		LdDescriptorSetLayout& operator=(const LdDescriptorSetLayout&) = delete;

	public: // functions
		VkDescriptorSetLayout getDescriptorSetLayout() const { return m_descriptorSetLayout; }

	public: // data
	private:
		LdDevice& m_device;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;
	};

	class LdDescriptorPool {
		friend class LdDescriptorWriter;

	public: // types
		class Builder {
		public:
			Builder(LdDevice& ldDevice) : ldDevice{ ldDevice } {}

		private:
			LdDevice& ldDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;

		public:
			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<LdDescriptorPool> build() const;
		};

	public: // constructors
		LdDescriptorPool(LdDevice& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~LdDescriptorPool();
		LdDescriptorPool(const LdDescriptorPool&) = delete;
		LdDescriptorPool& operator=(const LdDescriptorPool&) = delete;

	public: // functions
		bool allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
		void resetPool();

	private: // data
		LdDevice& m_device;
		VkDescriptorPool m_descriptorPool;
	};

	class LdDescriptorWriter {
	public: // constructors
		LdDescriptorWriter(LdDescriptorSetLayout& setLayout, LdDescriptorPool& pool);

	public: // functions
		LdDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		LdDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private: // data
		LdDescriptorSetLayout& m_setLayout;
		LdDescriptorPool& m_pool;
		std::vector<VkWriteDescriptorSet> m_writes;
	};

}  // namespace lve
#pragma once

#include "ld_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace ld {
	class LdDescriptorSetLayout {
	public:
		LdDescriptorSetLayout(LdDevice& ldDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~LdDescriptorSetLayout();
		LdDescriptorSetLayout(const LdDescriptorSetLayout&) = delete;
		LdDescriptorSetLayout& operator=(const LdDescriptorSetLayout&) = delete;

	public:
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
	private:
		LdDevice& ldDevice;
		VkDescriptorSetLayout descriptorSetLayout;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

		friend class LdDescriptorWriter;
	public:
		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }


	};

	class LdDescriptorPool {
	public:
		LdDescriptorPool(LdDevice& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~LdDescriptorPool();
		LdDescriptorPool(const LdDescriptorPool&) = delete;
		LdDescriptorPool& operator=(const LdDescriptorPool&) = delete;

	public:
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

	private:
		LdDevice& lveDevice;
		VkDescriptorPool descriptorPool;
		friend class LdDescriptorWriter;

	public:
		bool allocateDescriptorSet(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
		void resetPool();
	};

	class LdDescriptorWriter {
	public:
		LdDescriptorWriter(LdDescriptorSetLayout& setLayout, LdDescriptorPool& pool);
	private:
		LdDescriptorSetLayout& setLayout;
		LdDescriptorPool& pool;
		std::vector<VkWriteDescriptorSet> writes;

	public:
		LdDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		LdDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);
		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	};

}  // namespace lve
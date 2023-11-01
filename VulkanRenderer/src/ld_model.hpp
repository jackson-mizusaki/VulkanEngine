#pragma once

#include "ld_device.hpp"
#include "ld_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Ld
{
	class LdModel {
	public: // types
		struct Vertex {
			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}

			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Builder {
			void loadModel(const std::string& filepath);

			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};
		
	public: // constructors
		LdModel(LdDevice& device, const LdModel::Builder &builder);
		~LdModel();

		LdModel(const LdModel&) = delete;
		LdModel& operator=(const LdModel&) = delete;

	public: // functions
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::unique_ptr<LdModel> createModelFromFile(LdDevice& device, const std::string& filepath);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	public: // data
	private:
		LdDevice& m_device;

		std::unique_ptr<LdBuffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<LdBuffer> m_indexBuffer;
		uint32_t m_indexCount;
	};
}
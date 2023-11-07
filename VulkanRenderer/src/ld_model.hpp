#pragma once

#include "ld_device.hpp"
#include "ld_buffer.hpp"
#include "material.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace Ld {
	class Model {
	public: // types
		struct Vertex {
			bool operator==(const Vertex& other) const {
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv && texcoord == other.texcoord;
			}

			glm::vec3 position{};
			glm::vec4 color{};
			glm::vec3 normal{};
			glm::vec2 texcoord{};
			glm::vec2 uv{};
			glm::vec<4, unsigned short, glm::defaultp> joints{};
			glm::vec4 weights{};
			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Builder {
			//void loadModel(const std::string& filepath);

			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};
		
	public: // constructors
		Model(Device& device, const Model::Builder &builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

	public: // functions
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filepath);
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	public: // data
	private:
		Device& m_device;

		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount;
	};
}
#pragma once 

#include "ld_buffer.hpp"
#include "accessor.hpp"
#include "material.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <json.hpp>

#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class MeshPrimitive {
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
			void loadMeshPrimitive(const Accessor& accessor);
			void loadVertices(const Accessor& accessor);
			void loadIndices(const Accessor& accessor);
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		enum RenderingMode {
			Points,
			Lines,
			Line_Loop,
			Line_Strip,
			Triangles,
			Triangle_Strip,
			Triangle_Fan
		};
	public: // constructors
		MeshPrimitive(Builder& builder);
		MeshPrimitive(Device& device, const MeshPrimitive::Builder& builder);
		~MeshPrimitive();

		MeshPrimitive(const MeshPrimitive&) = delete;
		MeshPrimitive& operator=(const MeshPrimitive&) = delete;

	public: // functions
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::unique_ptr<MeshPrimitive> createMeshPrimitiveFromFile(Device& device, const std::string& filepath);

		
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	public: // data
	private:
		RenderingMode mode;
		Material* material;

		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount;
	};
}
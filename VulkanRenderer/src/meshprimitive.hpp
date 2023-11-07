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
				return position == other.position &&
					normal == other.normal &&
					tangent == other.tangent &&
					texcoord == other.texcoord &&
					color == other.color &&
					joints == other.joints && 
					weights == other.weights;
			}

			glm::vec3 position{};
			glm::vec3 normal{};
			glm::vec4 tangent{};
			glm::vec2 texcoord{};
			glm::vec4 color{};
			glm::vec<4, uint8_t, glm::defaultp> joints{};
			glm::vec4 weights{};
			static std::vector<VkVertexInputBindingDescription> getDefaultBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getDefaultAttributeDescriptions();
		};

		struct Builder {
			enum RenderingMode {
				Points,
				Lines,
				Line_Loop,
				Line_Strip,
				Triangles,
				Triangle_Strip,
				Triangle_Fan
			};
			//void loadMeshPrimitive(const Accessor& accessor);
			Builder();
			void loadVertices();
			void loadIndices();
			//std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions(Accessor& accessor);
			Buffer* buffer;
			Accessor* positionsAccessor;
			Accessor* normalsAccessor;
			Accessor* tangentsAccessor;
			std::map<uint32_t, Accessor*> texCoordsAccessor;
			std::map<uint32_t, Accessor*> colorsAccessor;
			std::map<uint32_t, Accessor*> jointsAccessor;
			std::map<uint32_t, Accessor*> weightsAccessor;
			RenderingMode renderMode = Triangles;
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};


	public: // constructors
		MeshPrimitive(Device& device, const MeshPrimitive::Builder& builder);
		~MeshPrimitive();

		MeshPrimitive(const MeshPrimitive&) = delete;
		MeshPrimitive& operator=(const MeshPrimitive&) = delete;

	public: // functions
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);
		VkBuffer getVertexBuffer() { return m_vertexBuffer->getBuffer(); }
		//static std::unique_ptr<MeshPrimitive> createMeshPrimitiveFromFile(Device& device, const std::string& filepath);

		
	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	public: // data
	private:
		Device& m_device;
		Material* material;

		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount;
	};
}
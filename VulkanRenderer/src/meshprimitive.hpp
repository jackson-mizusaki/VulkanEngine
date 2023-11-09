#pragma once 

#include "ld_buffer.hpp"
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
		enum Attribute {
			Position,
			Normal,
			Tangent,
			TexCoord,
			Color,
			Joints,
			Weights,
			Indices
		};
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
			enum IndexType {
				unsigned_Int,
				unsigned_Short,
				unsigned_Byte
			};

			//void loadMeshPrimitive(const Accessor& accessor);
			Builder();
			bool updateOrCheckCount(uint32_t count);
			const uint8_t* getData(Attribute attribute) const;
			uint32_t vertexCount = 0;
			uint32_t indexCount = 0;
			uint32_t attributeCount = 0;
			uint8_t colorSize = 3;
			std::map<Attribute, uint8_t*> accessorData;
			//static const int kPosition{ 0 };
			//static const int kNormal{ 1 };
			//static const int kTangent{ 2 };
			//static const int kTexCoord{ 3 };
			//static const int kColor{ 4 };
			//static const int kJoints{ 5 };
			//static const int kWeights{ 6 };
			//static const int kIndices{ 7 };
			IndexType indexType = unsigned_Int;
			RenderingMode renderMode = Triangles;
		};


	public: // constructors
		MeshPrimitive(Device& device, const MeshPrimitive::Builder& builder);
		~MeshPrimitive();
		MeshPrimitive(const MeshPrimitive&) = default;                // copy constructor
		MeshPrimitive& operator=(const MeshPrimitive&) = default;     // copy assignment
		MeshPrimitive(MeshPrimitive&&) noexcept = default;            // move constructor
		MeshPrimitive& operator=(MeshPrimitive&&) noexcept = default; // move assignment

	public: // functions
		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

		static std::unique_ptr<MeshPrimitive> createMeshPrimitiveFromBuilder(Device& device, const MeshPrimitive::Builder& builder);


	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

	public: // data
	private:
		Device& m_device;
		Material* material = nullptr;

		std::vector<VkDeviceSize> offsets;
		std::unique_ptr<Buffer> m_vertexBuffer;
		uint32_t m_vertexCount = 0;

		bool m_hasIndexBuffer = false;
		std::unique_ptr<Buffer> m_indexBuffer;
		uint32_t m_indexCount = 0;
	};
}
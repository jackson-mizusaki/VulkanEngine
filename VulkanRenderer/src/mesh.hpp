#pragma once  

#include "meshprimitive.hpp"

#include <json.hpp>


#include <vector>
#include <string>

using json = nlohmann::json;

namespace Ld {
	class Mesh {
	public: // functiosn
		Mesh(Device& device, size_t size);
		~Mesh() = default;
		void render(VkCommandBuffer commandBuffer);

		//void addPrimitive(MeshPrimitive* primitive);

		void addPrimitive(const MeshPrimitive::Builder& builder);
	public: // data
		std::string name = "unnamed mesh";
		int morphTargetCount = 0;
	private:
		Device& m_device;
		std::vector<std::unique_ptr<MeshPrimitive>> m_primitives{};
	};
}

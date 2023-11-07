#include "mesh.hpp"

namespace Ld {
	void Mesh::render(VkCommandBuffer commandBuffer)
	{
		for (auto& prim : primitives)
		{
			prim.bind(commandBuffer);
			prim.draw(commandBuffer);
		}
	}
}
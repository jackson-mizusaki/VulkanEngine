#include "mesh.hpp"

namespace Ld {

	Mesh::Mesh(Device& device, size_t size)
		: m_device{ device }, m_primitives{ size }
	{
	}

	void Mesh::render(VkCommandBuffer commandBuffer)
	{
		for (auto& prim : m_primitives)
		{
			prim->bind(commandBuffer);
			prim->draw(commandBuffer);
		}
	}
	//void Mesh::addPrimitive(MeshPrimitive* primitive)
	//{
	//	assert(primitive += nullptr && "mesh primitive pointer is null");
	//	try
	//	{
	//		m_primitives.push_back(primitive);
	//	}
	//	catch (std::exception e)
	//	{
	//		throw e;
	//	}
	//}
	void Mesh::addPrimitive(const MeshPrimitive::Builder& builder)
	{
		try
		{
			m_primitives.push_back(MeshPrimitive::createMeshPrimitiveFromBuilder(m_device, builder));
		}
		catch (std::exception e)
		{
			throw e;
		}
	}
}
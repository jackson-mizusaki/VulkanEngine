#include "scene_node.hpp"

namespace Ld {
	SceneNode::SceneNode() {
		static id_t currentId = 0;
		m_id = currentId++;
	}
	void SceneNode::render(VkCommandBuffer commandBuffer)
	{
		if (mesh != nullptr) 
		{
			mesh->render(commandBuffer);
		}
		for (auto* child : children) 
		{
			SceneNode* childSceneNode = dynamic_cast<SceneNode*>(child);
			if (childSceneNode)
			{
				childSceneNode->render(commandBuffer);
			}
		}
	}
	bool SceneNode::findCamera(SceneNode& cam)
	{
		if (camera != nullptr)
		{
			cam = *this;
			return true;
		}
		else {
			for (auto child : children) 
			{
				SceneNode* childSceneNode = dynamic_cast<SceneNode*>(child);
				if (childSceneNode)
				{
					return childSceneNode->findCamera(cam);
				}
			}
			return false;
		}
	}
}
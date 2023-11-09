#include "scene.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <stdexcept>



namespace Ld {
	Scene::Scene()
	{
	}
	
	Scene::Scene(std::string& filepath)
	{
		//loadScene(filepath);
	}

	bool Scene::findCameraNode(SceneNode& cameraNode)
	{
		for (auto& node : nodes) {
			SceneNode* childSceneNode = dynamic_cast<SceneNode*>(node);
			return childSceneNode->findCamera(cameraNode);
		 }
		return false;
	}

	void Scene::render(VkCommandBuffer commandBuffer)
	{
		for (auto& node : nodes) {
			SceneNode* childSceneNode = dynamic_cast<SceneNode*>(node);
			childSceneNode->render(commandBuffer);
		}
	}

	void Scene::addNode(SceneNode* node)
	{
		nodes.push_back(node);
	}

}
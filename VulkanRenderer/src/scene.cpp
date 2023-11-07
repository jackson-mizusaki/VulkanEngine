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
			return node->findCamera(cameraNode);
		 }
		return false;
	}

	void Scene::render(VkCommandBuffer commandBuffer)
	{
		for (auto& node : nodes) {
			node->render(commandBuffer);
		}
	}

}
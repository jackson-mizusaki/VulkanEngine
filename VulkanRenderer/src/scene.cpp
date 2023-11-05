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
		loadScene(filepath);
	}

	void Scene::loadScene(const std::string& filepath)
	{
		// iterate through objects saved in scene file
		std::ifstream f(filepath.c_str());


	}

}
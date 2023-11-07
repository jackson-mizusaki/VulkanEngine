#pragma once

#include "ld_window.hpp"
#include "ld_device.hpp"
#include "ld_renderer.hpp"
#include "ld_descriptors.hpp"
#include "vk_mem_alloc.h"
#include "scene.hpp"
#include <memory>
#include <vector>

namespace Ld {
	class App {
	public: // constructors
		App();
		~App();
		App(const App&) = delete;
		App& operator=(const App&) = delete;

	public: // functions
		void run();
		void addScene(Scene& scene);
		void setDefaultScene(uint32_t sceneIndex) { m_defaultScene = sceneIndex; }
	private:
		//void loadGameObjects();

	public: // data
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	private:
		Window m_window{ WIDTH, HEIGHT, "App Window" };
		Device m_device{ m_window };
		Renderer m_renderer{ m_window, m_device };

		int m_defaultScene = -1;
		std::unique_ptr<DescriptorPool> m_globalPool{};
		std::vector<Scene> scenes;
		//GameObject::Map m_gameObjects;
	};
}

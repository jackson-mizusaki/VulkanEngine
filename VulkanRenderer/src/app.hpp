#pragma once

#include "ld_window.hpp"
#include "ld_device.hpp"
#include "ld_renderer.hpp"
#include "ld_model.hpp"
#include "ld_game_object.hpp"
#include "ld_descriptors.hpp"
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
	private:
		void loadGameObjects();

	public: // data
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	private:
		LdWindow m_window{ WIDTH, HEIGHT, "App Window" };
		LdDevice m_device{ m_window };
		LdRenderer m_renderer{ m_window, m_device };

		std::unique_ptr<LdDescriptorPool> m_globalPool{};
		LdGameObject::Map m_gameObjects;
	};
}

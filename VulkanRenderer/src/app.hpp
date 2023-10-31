#pragma once

#include "ld_window.hpp"
#include "ld_device.hpp"
#include "ld_renderer.hpp"
#include "ld_model.hpp"
#include "ld_game_object.hpp"
#include "ld_descriptors.hpp"
#include <memory>
#include <vector>

namespace ld {
	class App {
	public:
		App();
		~App();
		App(const App&) = delete;
		App& operator=(const App&) = delete;

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
	private:
		LdWindow ldWindow{ WIDTH, HEIGHT, "App Window" };
		LdDevice ldDevice{ ldWindow };
		LdRenderer ldRenderer{ ldWindow, ldDevice };

		std::unique_ptr<LdDescriptorPool> globalPool{};
		LdGameObject::Map gameObjects;
	public:
		void run();

	private:
		void loadGameObjects();
	};
}

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Ld {
	class Window {
	public: // constructors
		Window(int w, int h, std::string name);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

	public: // functions
		bool shouldClose() { return glfwWindowShouldClose(m_window); }
		bool wasWindowResized() { return m_isFramebufferResized; }
		void resetWindowResizedFlag() { m_isFramebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return m_window; }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

	public: // data
	private:
		int m_width;
		int m_height;
		bool m_isFramebufferResized = false;

		std::string m_windowName;
		GLFWwindow* m_window;	
	};
}
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Ld {
	class LdWindow {
	public: // constructors
		LdWindow(int w, int h, std::string name);
		~LdWindow();
		LdWindow(const LdWindow&) = delete;
		LdWindow& operator=(const LdWindow&) = delete;

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
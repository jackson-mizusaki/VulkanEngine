#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace ld {
	class LdWindow {

	public:
		LdWindow(int w, int h, std::string name);
		~LdWindow();

		LdWindow(const LdWindow&) = delete;
		LdWindow& operator=(const LdWindow&) = delete;

	private:
		int width;
		int height;
		bool isFramebufferResized = false;

		std::string windowName;
		GLFWwindow* window;

	public:
		bool shouldClose() { return glfwWindowShouldClose(window); }
		bool wasWindowResized() { return isFramebufferResized; }
		void resetWindowResizedFlag() { isFramebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();
	
	};
}
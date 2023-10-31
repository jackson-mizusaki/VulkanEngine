#include "ld_window.hpp"

#include <stdexcept>

namespace ld
{
	LdWindow::LdWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name }
	{
		initWindow();
	}

	LdWindow::~LdWindow()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LdWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	void LdWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto ldWindow = reinterpret_cast<LdWindow*>(glfwGetWindowUserPointer(window));
		ldWindow->isFramebufferResized = true;
		ldWindow->width = width;
		ldWindow->height = height;
	}

	void LdWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
}
#include "ld_window.hpp"

#include <stdexcept>

namespace Ld
{
	LdWindow::LdWindow(int w, int h, std::string name) : m_width{ w }, m_height{ h }, m_windowName{ name }
	{
		initWindow();
	}

	LdWindow::~LdWindow()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	void LdWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface");
		}
	}

	void LdWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto ldWindow = reinterpret_cast<LdWindow*>(glfwGetWindowUserPointer(window));
		ldWindow->m_isFramebufferResized = true;
		ldWindow->m_width = width;
		ldWindow->m_height = height;
	}

	void LdWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
	}
}
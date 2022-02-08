#include "pgs_window.hpp"

// std
#include <stdexcept>

namespace pgs
{

PgsWindow::PgsWindow(int w, int h, std::string name) : m_width{w}, m_height{h}, m_windowName{name}
{
	initWindow();
}

PgsWindow::~PgsWindow()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void PgsWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

void PgsWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
{
	if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to craete window surface");
	}
}

void PgsWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
	auto pgsWindow = reinterpret_cast<PgsWindow *>(glfwGetWindowUserPointer(window));
	pgsWindow->m_framebufferResized = true;
	pgsWindow->m_width = width;
	pgsWindow->m_height = height;
}

} // namespace pgs

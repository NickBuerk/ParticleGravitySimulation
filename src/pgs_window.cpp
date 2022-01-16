#include "pgs_window.hpp"

// std
#include <stdexcept>

namespace pgs
{

PgsWindow::PgsWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
{
	initWindow();
}

PgsWindow::~PgsWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void PgsWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void PgsWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to craete window surface");
	}
}

void PgsWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
	auto pgsWindow = reinterpret_cast<PgsWindow *>(glfwGetWindowUserPointer(window));
	pgsWindow->framebufferResized = true;
	pgsWindow->width = width;
	pgsWindow->height = height;
}

} // namespace pgs

#include "SgWindow.hpp"

#include <stdexcept>

namespace SunGlassEngine {
	SgWindow::SgWindow(int widthIn, int heightIn, std::string nameIn)
		: width{ widthIn }, height{ heightIn }, windowName{ nameIn } {
		initWindow();
	}

	SgWindow::~SgWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void SgWindow::initWindow() {
		glfwInit(); // initialize glfw library
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // don't create OpenGL context
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // disable automatic window resizing

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void SgWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}

	void SgWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto sgWindow = reinterpret_cast<SgWindow*>(glfwGetWindowUserPointer(window));
		sgWindow->framebufferResized = true;
		sgWindow->width = width;
		sgWindow->height = height;
	}
} // namespace SunGlassEngine
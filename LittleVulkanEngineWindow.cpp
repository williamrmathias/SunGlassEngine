#include "LittleVulkanEngineWindow.hpp"

namespace LittleVulkanEngine {
	LveWindow::LveWindow(int widthIn, int heightIn, std::string nameIn)
		: width{ widthIn }, height{ heightIn }, windowName{ nameIn } {
		initWindow();
	}

	LveWindow::~LveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LveWindow::initWindow() {
		glfwInit(); // initialize glfw library
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // don't create OpenGL context
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // disable automatic window resizing

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}
} // namespace LittleVulkanEngine
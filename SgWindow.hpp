#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace SunGlassEngine {

	class SgWindow {
	public:
		SgWindow(int widthIn, int heightIn, std::string nameIn);
		~SgWindow();

		SgWindow(const SgWindow&) = delete;
		SgWindow& operator=(const SgWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() {
			return{ 
				static_cast<uint32_t>(width), static_cast<uint32_t>(height)
			};
		};
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int width;
		int height;
		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
	};

}
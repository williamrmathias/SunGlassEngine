#include "FirstApp.hpp"

namespace LittleVulkanEngine {
	void FirstApp::run() {
		while (!lveWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
} // namespace LittleVulkanEngine
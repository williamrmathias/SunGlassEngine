#pragma once

#include "LittleVulkanEngineWindow.hpp"

namespace LittleVulkanEngine {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
	};
} // namespace LittleVulkanEngine
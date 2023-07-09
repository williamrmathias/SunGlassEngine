#pragma once

#include "LittleVulkanEngineDevice.hpp"
#include "LittleVulkanEngineGameObject.hpp"
#include "LittleVulkanEngineWindow.hpp"
#include "LittleVulkanEngineRenderer.hpp"

// std
#include <memory>
#include <vector>

namespace LittleVulkanEngine {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{ lveWindow, lveDevice };
		std::vector<LveGameObject> gameObjects;
	};
} // namespace LittleVulkanEngine
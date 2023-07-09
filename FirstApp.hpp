#pragma once

#include "LittleVulkanEngineDevice.hpp"
#include "LittleVulkanEngineModel.hpp"
#include "LittleVulkanEngineGameObject.hpp"
#include "LittleVulkanEnginePipeline.hpp"
#include "LittleVulkanEngineSwapChain.hpp"
#include "LittleVulkanEngineWindow.hpp"

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{ lveWindow };
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<LveGameObject> gameObjects;
	};
} // namespace LittleVulkanEngine
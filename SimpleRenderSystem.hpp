#pragma once

#include "LittleVulkanEngineCamera.hpp"
#include "LittleVulkanEngineDevice.hpp"
#include "LittleVulkanEngineGameObject.hpp"
#include "LittleVulkanEnginePipeline.hpp"

// std
#include <memory>
#include <vector>

namespace LittleVulkanEngine {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(
			VkCommandBuffer commandBuffer,
			std::vector<LveGameObject>& gameObjects,
			const LveCamera& camera
		);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace LittleVulkanEngine
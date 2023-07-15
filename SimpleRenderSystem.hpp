#pragma once

#include "LittleVulkanEngineCamera.hpp"
#include "LittleVulkanEngineDevice.hpp"
#include "LittleVulkanEngineFrameInfo.hpp"
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
			FrameInfo& frameInfo,
			std::vector<LveGameObject>& gameObjects
		);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LveDevice& lveDevice;
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace LittleVulkanEngine
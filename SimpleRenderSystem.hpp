#pragma once

#include "SgCamera.hpp"
#include "SgDevice.hpp"
#include "SgFrameInfo.hpp"
#include "SgGameObject.hpp"
#include "SgPipeline.hpp"


// std
#include <memory>
#include <vector>

namespace SunGlassEngine {
	class SimpleRenderSystem {
	public:

		SimpleRenderSystem(SgDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(
			FrameInfo& frameInfo,
			std::vector<SgGameObject>& gameObjects
		);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		SgDevice& sgDevice;
		std::unique_ptr<SgPipeline> sgPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace SunGlassEngine
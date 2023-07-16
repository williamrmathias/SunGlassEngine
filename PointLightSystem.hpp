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
	class PointLightSystem {
	public:

		PointLightSystem(SgDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);

		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		SgDevice& sgDevice;
		std::unique_ptr<SgPipeline> sgPipeline;
		VkPipelineLayout pipelineLayout;
	};
} // namespace SunGlassEngine
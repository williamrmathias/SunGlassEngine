#include "PointLightSystem.hpp"

// libs
#define GLF_FORCE_RADIANS // angles specified in randians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // depth buffer ranges from 0 to 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>

namespace SunGlassEngine {

	PointLightSystem::PointLightSystem(SgDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: sgDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem() {
		vkDestroyPipelineLayout(sgDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		// VkPushConstantRange pushConstantRange{};
		// use in both vertex and fragment shader
		//pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		//pushConstantRange.offset = 0;
		//pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayout{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(
			sgDevice.device(),
			&pipelineLayoutInfo,
			nullptr,
			&pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	};

	void PointLightSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		// use width and height from swap chain, not window
		PipelineConfigInfo pipelineConfig{};
		SgPipeline::defaultPipelineConfigInfo(pipelineConfig);

		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.attributeDescriptions.clear();

		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		sgPipeline = std::make_unique<SgPipeline>(
			sgDevice,
			"shaders/PointLight.vert.spv",
			"shaders/PointLight.frag.spv",
			pipelineConfig);
	}

	void PointLightSystem::render(FrameInfo& frameInfo) {

		// render
		sgPipeline->bind(frameInfo.commandBuffer);

		auto projectionView = frameInfo.camera.getProjection() *
			frameInfo.camera.getView();

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
} // namespace SunGlassEngine
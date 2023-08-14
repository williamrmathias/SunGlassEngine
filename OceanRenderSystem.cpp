#include "OceanRenderSystem.hpp"

// libs
#define GLF_FORCE_RADIANS // angles specified in randians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // depth buffer ranges from 0 to 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>

namespace SunGlassEngine {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.0f }; // identity
		glm::mat4 normalMatrix{ 1.0f };
	};

	OceanRenderSystem::OceanRenderSystem(SgDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
		: sgDevice{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	OceanRenderSystem::~OceanRenderSystem() {
		vkDestroyPipelineLayout(sgDevice.device(), pipelineLayout, nullptr);
	}

	void OceanRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {

		VkPushConstantRange pushConstantRange{};
		// use in both vertex and fragment shader
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayout{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(
			sgDevice.device(),
			&pipelineLayoutInfo,
			nullptr,
			&pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	};

	void OceanRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		// use width and height from swap chain, not window
		PipelineConfigInfo pipelineConfig{};
		SgPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		sgPipeline = std::make_unique<SgPipeline>(
			sgDevice,
			"shaders/Ocean.vert.spv",
			"shaders/Ocean.frag.spv",
			pipelineConfig);
	}

	void OceanRenderSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
		int waveIndex = 0;
		for (auto& keyValue : frameInfo.gameObjects) {
			auto& obj = keyValue.second;
			if (obj.wave == nullptr) continue;

			assert(waveIndex < MAX_WAVES && "Number of waves exceed maximum");

			// Calculate frequency
			float waveFrequency = glm::sqrt(9.8 * obj.wave->waveNumber);

			// update phase
			float phase = glm::mod(
				obj.wave->wavePhase + waveFrequency * frameInfo.frameTime,
				glm::two_pi<float>());

			obj.wave->wavePhase = phase;

			ubo.waves[waveIndex].waveVector = obj.wave->waveDirection * obj.wave->waveNumber;
			ubo.waves[waveIndex].amplitude = obj.wave->waveAmplitude;
			ubo.waves[waveIndex].phase = obj.wave->wavePhase;
			waveIndex += 1;
		}
		ubo.numWaves = waveIndex;
	}

	void OceanRenderSystem::render(FrameInfo& frameInfo) {

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

		for (auto& keyValue : frameInfo.gameObjects) {
			auto& obj = keyValue.second;

			if (obj.wave == nullptr) continue;

			SimplePushConstantData push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
} // namespace SunGlassEngine
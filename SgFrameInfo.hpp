#pragma once

#include "SgCamera.hpp"
#include "SgGameObject.hpp"

// lib
#include <vulkan/vulkan.h>

namespace SunGlassEngine {

#define MAX_LIGHTS 10
#define MAX_WAVES 3

	struct PointLight {
		glm::vec4 position{}; // .w = N/A
		glm::vec4 color{};
	};

	struct Wave {
		glm::vec2 waveVector{}; // direction of wave * wave number
		float amplitude;
		float phase;
	};

	struct GlobalUbo {
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, 0.02f }; // .w is intensity

		PointLight pointLights[MAX_LIGHTS];
		Wave waves[MAX_WAVES];

		int numLights;
		int numWaves;
	};

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		SgCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		SgGameObject::Map& gameObjects;
	};

} // namespace SunGlassEngine
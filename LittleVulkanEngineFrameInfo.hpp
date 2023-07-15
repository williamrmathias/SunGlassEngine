#pragma once

#include "LittleVulkanEngineCamera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace LittleVulkanEngine {

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		LveCamera& camera;
	};

} // namespace LittleVulkanEngine
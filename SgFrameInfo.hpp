#pragma once

#include "SgCamera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace SunGlassEngine {

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		SgCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};

} // namespace SunGlassEngine
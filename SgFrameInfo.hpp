#pragma once

#include "SgCamera.hpp"
#include "SgGameObject.hpp"

// lib
#include <vulkan/vulkan.h>

namespace SunGlassEngine {

	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		SgCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		SgGameObject::Map& gameObjects;
	};

} // namespace SunGlassEngine
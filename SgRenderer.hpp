#pragma once

#include "SgDevice.hpp"
#include "SgSwapChain.hpp"
#include "SgWindow.hpp"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace SunGlassEngine {
	class SgRenderer {
	public:

		SgRenderer(SgWindow& window, SgDevice& device);
		~SgRenderer();

		SgRenderer(const SgRenderer&) = delete;
		SgRenderer& operator=(const SgRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return sgSwapChain->getRenderPass(); }

		float getAspectRatio() const { return sgSwapChain->extentAspectRatio(); }

		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();

		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		SgWindow& sgWindow;
		SgDevice& sgDevice;
		std::unique_ptr<SgSwapChain> sgSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
} // namespace SunGlassEngine
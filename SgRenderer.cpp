#include "SgRenderer.hpp"

// std
#include <stdexcept>
#include <array>

namespace SunGlassEngine {

	SgRenderer::SgRenderer(SgWindow& window, SgDevice& device)
		: sgWindow{ window }, sgDevice(device) {
		recreateSwapChain();
		createCommandBuffers();
	}

	SgRenderer::~SgRenderer() {
		freeCommandBuffers();
	}

	void SgRenderer::recreateSwapChain() {
		auto extent = sgWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			// engine will pause if one dimension is sizeless
			extent = sgWindow.getExtent();
			glfwWaitEvents();
		}

		// wait till old swap chain is no longer used before
		// making the new one
		vkDeviceWaitIdle(sgDevice.device());

		if (sgSwapChain == nullptr) {
			sgSwapChain = std::make_unique<SgSwapChain>(sgDevice, extent);
		}
		else {
			std::shared_ptr<SgSwapChain> oldSwapChain = std::move(sgSwapChain);
			sgSwapChain = std::make_unique<SgSwapChain>(
				sgDevice,
				extent,
				oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*sgSwapChain.get())) {
				throw std::runtime_error("Swap chain image (or depth) format has changed");
			}
		}
	}

	void SgRenderer::createCommandBuffers() {
		// number of command buffers should be equal to number of
		// framebuffers
		commandBuffers.resize(SgSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = sgDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(sgDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}
	};

	void SgRenderer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			sgDevice.device(),
			sgDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());

		commandBuffers.clear();
	}

	VkCommandBuffer SgRenderer::beginFrame() {
		assert(!isFrameStarted && "Can't call beginFrame while frame already in progress");

		// acquire the next frame to render to
		auto result = sgSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			// window has been resized
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer");
		}

		return commandBuffer;
	}

	void SgRenderer::endFrame() {
		assert(isFrameStarted && "Can't call endFrame while frame not in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		// submit command buffer to device graphics queue
		auto result = sgSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR
			|| result == VK_SUBOPTIMAL_KHR
			|| sgWindow.wasWindowResized()) {
			sgWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}

		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % SgSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void SgRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't begin render pass on command buffer from a different frame"
		);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = sgSwapChain->getRenderPass();
		renderPassInfo.framebuffer = sgSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		// must use swapchain extent and not window extent
		renderPassInfo.renderArea.extent = sgSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(sgSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(sgSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, sgSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void SgRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == getCurrentCommandBuffer() &&
			"Can't end render pass on command buffer from a different frame"
		);

		vkCmdEndRenderPass(commandBuffer);
	}
} // namespace SunGlassEngine
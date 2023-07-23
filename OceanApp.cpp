#include "OceanApp.hpp"

#include "KeyboardMovementController.hpp"
#include "SgBuffer.hpp"
#include "SgCamera.hpp"
#include "SimpleRenderSystem.hpp"
#include "PointLightSystem.hpp"
#include "OceanRenderSystem.hpp"

// libs
#define GLF_FORCE_RADIANS // angles specified in randians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // depth buffer ranges from 0 to 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>

namespace SunGlassEngine {

	OceanApp::OceanApp() {
		globalPool = SgDescriptorPool::Builder(sgDevice)
			.setMaxSets(SgSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SgSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	OceanApp::~OceanApp() {}

	void OceanApp::run() {

		std::vector<std::unique_ptr<SgBuffer>> uboBuffers(SgSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uboBuffers.size(); i++) {
			uboBuffers[i] = std::make_unique<SgBuffer>(
				sgDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = SgDescriptorSetLayout::Builder(sgDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(SgSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			SgDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem(
			sgDevice,
			sgRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout());

		PointLightSystem pointLightSystem(
			sgDevice,
			sgRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout());

		OceanRenderSystem oceanRenderSystem(
			sgDevice,
			sgRenderer.getSwapChainRenderPass(),
			globalSetLayout->getDescriptorSetLayout());

		SgCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5));

		auto viewerObject = SgGameObject::createGameObject();
		viewerObject.transform.translation.z = -2.5f;
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!sgWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float,
				std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(sgWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(
				viewerObject.transform.translation, viewerObject.transform.rotation
			);

			float aspect = sgRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if (auto commandBuffer = sgRenderer.beginFrame()) {
				int frameIndex = sgRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				oceanRenderSystem.update(frameInfo, ubo);
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// render
				sgRenderer.beginSwapChainRenderPass(commandBuffer);

				// must render all solid objects before semi transparent objects
				simpleRenderSystem.renderGameObjects(frameInfo);
				oceanRenderSystem.render(frameInfo);
				pointLightSystem.render(frameInfo);
				sgRenderer.endSwapChainRenderPass(commandBuffer);
				sgRenderer.endFrame();
			}
		}

		// block CPU until GPU operations are completed
		vkDeviceWaitIdle(sgDevice.device());
	}

	void OceanApp::loadGameObjects() {

		std::shared_ptr<SgModel> sgModel = SgModel::createModelFromFile(
			sgDevice, "models/grid64.obj");
		auto ocean = SgGameObject::createGameObject();
		ocean.model = sgModel;
		ocean.transform.translation = { 0.5f, 0.5f, 0.f };
		ocean.transform.scale = glm::vec3{ 0.1f, 0.1f, 0.1f };
		ocean.wave = std::make_unique<WaveComponent>();
		gameObjects.emplace(ocean.getId(), std::move(ocean));
	}

} // namespace SunGlassEngine
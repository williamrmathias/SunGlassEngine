#include "FirstApp.hpp"

#include "KeyboardMovementController.hpp"
#include "LittleVulkanEngineBuffer.hpp"
#include "LittleVulkanEngineCamera.hpp"
#include "SimpleRenderSystem.hpp"

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

namespace LittleVulkanEngine {

	struct GlobalUbo {
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	FirstApp::FirstApp() {
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {

		LveBuffer globalUboBuffer{
			lveDevice,
			sizeof(GlobalUbo),
			LveSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			lveDevice.properties.limits.minUniformBufferOffsetAlignment
		};
		globalUboBuffer.map();

		SimpleRenderSystem simpleRenderSystem(lveDevice, lveRenderer.getSwapChainRenderPass());
		LveCamera camera{};
		camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5));

		auto viewerObject = LveGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!lveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float,
				std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(lveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(
				viewerObject.transform.translation, viewerObject.transform.rotation
			);

			float aspect = lveRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
			
			if (auto commandBuffer = lveRenderer.beginFrame()) {
				int frameIndex = lveRenderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);
			
				// render
				lveRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				lveRenderer.endSwapChainRenderPass(commandBuffer);
				lveRenderer.endFrame();
			}
		}

		// block CPU until GPU operations are completed
		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<LveModel> lveModel = LveModel::createModelFromFile(
			lveDevice, "models/smooth_vase.obj");

		auto gameObj = LveGameObject::createGameObject();
		gameObj.model = lveModel;
		gameObj.transform.translation = { 0.f, 0.5f, 2.5f };
		gameObj.transform.scale = glm::vec3{ 3.f, 1.5, 3.f };
		gameObjects.push_back(std::move(gameObj));
	}
} // namespace LittleVulkanEngine
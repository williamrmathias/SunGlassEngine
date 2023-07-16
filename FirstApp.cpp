#include "FirstApp.hpp"

#include "KeyboardMovementController.hpp"
#include "SgBuffer.hpp"
#include "SgCamera.hpp"
#include "SimpleRenderSystem.hpp"
#include "PointLightSystem.hpp"

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

	FirstApp::FirstApp() {
		globalPool = SgDescriptorPool::Builder(sgDevice)
			.setMaxSets(SgSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SgSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	FirstApp::~FirstApp() {}

	void FirstApp::run() {

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
				pointLightSystem.update(frameInfo, ubo);
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();
			
				// render
				sgRenderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo);
				pointLightSystem.render(frameInfo);
				sgRenderer.endSwapChainRenderPass(commandBuffer);
				sgRenderer.endFrame();
			}
		}

		// block CPU until GPU operations are completed
		vkDeviceWaitIdle(sgDevice.device());
	}

	void FirstApp::loadGameObjects() {
		std::shared_ptr<SgModel> sgModel = SgModel::createModelFromFile(
			sgDevice, "models/flat_vase.obj");

		auto flatVase = SgGameObject::createGameObject();
		flatVase.model = sgModel;
		flatVase.transform.translation = { -0.5f, 0.5f, 0.f };
		flatVase.transform.scale = glm::vec3{ 3.f, 1.5, 3.f };
		gameObjects.emplace(flatVase.getId(), std::move(flatVase));

		sgModel = SgModel::createModelFromFile(
			sgDevice, "models/smooth_vase.obj");

		auto smoothVase = SgGameObject::createGameObject();
		smoothVase.model = sgModel;
		smoothVase.transform.translation = { 0.5f, 0.5f, 0.f };
		smoothVase.transform.scale = glm::vec3{ 3.f, 1.5, 3.f };
		gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

		sgModel = SgModel::createModelFromFile(
			sgDevice, "models/quad.obj");

		auto floor = SgGameObject::createGameObject();
		floor.model = sgModel;
		floor.transform.translation = { 0.f, 0.5f, 0.f };
		floor.transform.scale = glm::vec3{ 3.f, 1.f, 3.f };
		gameObjects.emplace(floor.getId(), std::move(floor));

		std::vector<glm::vec3> lightColors{
			 {1.f, .1f, .1f},
			 {.1f, .1f, 1.f},
			 {.1f, 1.f, .1f},
			 {1.f, 1.f, .1f},
			 {.1f, 1.f, 1.f},
			 {1.f, 1.f, 1.f}  //
		};

		for (int i = 0; i < lightColors.size(); i++) {
			auto pointLight = SgGameObject::makePointLight(0.2f);
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.f), 
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.f, -1.f, 0.f });
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, -1.f));
			gameObjects.emplace(pointLight.getId(), std::move(pointLight));
		}
	}
} // namespace SunGlassEngine
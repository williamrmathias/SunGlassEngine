#pragma once

#include "LittleVulkanEngineBuffer.hpp"
#include "LittleVulkanEngineDevice.hpp"

// libs
#define GLF_FORCE_RADIANS // angles specified in randians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // depth buffer ranges from 0 to 1
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace LittleVulkanEngine {
	class LveModel {
		// this class servers to read in model data
		// and export it to the gpu device

	public:

		struct Vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const {
				return position == other.position &&
					color == other.color &&
					normal == other.normal &&
					uv == other.uv;
			}
		};

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		LveModel(LveDevice& device, const LveModel::Builder& builder);
		~LveModel();

		static std::unique_ptr<LveModel> createModelFromFile(
			LveDevice& device, const std::string& filepath);

		LveModel(const LveModel&) = delete;
		LveModel& operator=(const LveModel&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		void draw(VkCommandBuffer commandBuffer);

	private:

		void createVertexBuffers(const std::vector<Vertex>& vertices);
		void createIndexBuffers(const std::vector<uint32_t>& indices);

		LveDevice& lveDevice; // device must outlive model

		std::unique_ptr<LveBuffer> vertexBuffer;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		std::unique_ptr<LveBuffer> indexBuffer;
		uint32_t indexCount;
	};
}

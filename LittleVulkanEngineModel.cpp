#include "LittleVulkanEngineModel.hpp"
#include "LittleVulkanEngineUtils.hpp"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjectLoader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std {

	template <>
	struct hash<LittleVulkanEngine::LveModel::Vertex> {
		size_t operator()(LittleVulkanEngine::LveModel::Vertex const& vertex) const {
			size_t seed = 0;
			LittleVulkanEngine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

}

namespace LittleVulkanEngine {
	
	LveModel::LveModel(LveDevice& device, const LveModel::Builder& builder)
		:lveDevice{device} {
		createVertexBuffers(builder.vertices);
		createIndexBuffers(builder.indices);
	}

	LveModel::~LveModel() {}

	std::unique_ptr<LveModel> LveModel::createModelFromFile(
		LveDevice& device, const std::string& filepath) {
		Builder builder{};
		builder.loadModel(filepath);
		return std::make_unique<LveModel>(device, builder);
	}

	void LveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		LveBuffer stagingBuffer{
			lveDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)vertices.data());

		vertexBuffer = std::make_unique<LveBuffer>(
			lveDevice,
			vertexSize,
			vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		lveDevice.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer->getBuffer(), bufferSize);
	}

	void LveModel::createIndexBuffers(const std::vector<uint32_t>& indices) {
		indexCount = static_cast<uint32_t>(indices.size());
		hasIndexBuffer = indexCount > 0;

		if (!hasIndexBuffer) return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;
		uint32_t indexSize = sizeof(indices[0]);

		LveBuffer stagingBuffer{
			lveDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void*)indices.data());

		indexBuffer = std::make_unique<LveBuffer>(
			lveDevice,
			indexSize,
			indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		lveDevice.copyBuffer(stagingBuffer.getBuffer(), indexBuffer->getBuffer(), bufferSize);
	}

	void LveModel::draw(VkCommandBuffer commandBuffer) {
		if (hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
		}
		else {
			vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
		}
	}

	void LveModel::bind(VkCommandBuffer commandBuffer) {
		VkBuffer buffers[] = { vertexBuffer->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({
			0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({
			1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({
			2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({
			3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void LveModel::Builder::loadModel(const std::string& filepath) {
		tinyobj::attrib_t attribute;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;

		if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, filepath.c_str())) {
			throw std::runtime_error(warning + error);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		// loop through each face in the model
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};

				// check if index provided
				if (index.vertex_index >= 0) {
					vertex.position = {
						attribute.vertices[3 * index.vertex_index + 0],
						attribute.vertices[3 * index.vertex_index + 1],
						attribute.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attribute.colors[3 * index.vertex_index + 0],
						attribute.colors[3 * index.vertex_index + 1],
						attribute.colors[3 * index.vertex_index + 2]
					};
				}

				// check if normal provided
				if (index.normal_index >= 0) {
					vertex.normal = {
						attribute.normals[3 * index.normal_index + 0],
						attribute.normals[3 * index.normal_index + 1],
						attribute.normals[3 * index.normal_index + 2]
					};
				}


				// check if texture coords provided
				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attribute.texcoords[2 * index.texcoord_index + 0],
						attribute.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

} // LittleVulkanEngine
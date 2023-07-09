#pragma once

#include "LittleVulkanEngineModel.hpp"

// std
#include <memory>

namespace LittleVulkanEngine {

	struct Transform2dComponent {
		glm::vec2 translation{}; // position offset
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;

		glm::mat2 mat2() { 
			const float sin = glm::sin(rotation);
			const float cos = glm::cos(rotation);

			glm::mat2 rotationMatrix{ {cos, sin}, {-sin, cos} };

			glm::mat2 scaleMatrix{ {scale.x, 0.0f}, {0.0f, scale.y} };
			return rotationMatrix * scaleMatrix;
		}
	};

	class LveGameObject {
	public:
		using id_t = unsigned int;

		static LveGameObject createGameObject() {
			static id_t currentId = 0;
			return LveGameObject(currentId++);
		}

		LveGameObject(const LveGameObject&) = delete;
		LveGameObject& operator=(const LveGameObject&) = delete;
		LveGameObject(LveGameObject&&) = default;
		LveGameObject& operator=(LveGameObject&&) = default;

		const id_t getId() { return id; }

		std::shared_ptr<LveModel> model{};
		glm::vec3 color{};
		Transform2dComponent transform2d{};

	private:
		LveGameObject(id_t objectId) : id(objectId) {}

		id_t id;
	};

} // namespace Little Vulkan Engine
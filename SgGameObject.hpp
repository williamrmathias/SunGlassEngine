#pragma once

#include "SgModel.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace SunGlassEngine {

	struct TransformComponent {
		glm::vec3 translation{}; // position offset
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix = translate * Ry * Rx * Rz * scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class SgGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, SgGameObject>;

		static SgGameObject createGameObject() {
			static id_t currentId = 0;
			return SgGameObject(currentId++);
		}

		SgGameObject(const SgGameObject&) = delete;
		SgGameObject& operator=(const SgGameObject&) = delete;
		SgGameObject(SgGameObject&&) = default;
		SgGameObject& operator=(SgGameObject&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<SgModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		SgGameObject(id_t objectId) : id(objectId) {}

		id_t id;
	};

} // namespace Little Vulkan Engine
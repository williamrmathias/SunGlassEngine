#include "KeyboardMovementController.hpp"

namespace SunGlassEngine {

	void KeyboardMovementController::moveInPlaneXZ(
		GLFWwindow* window, float dt, SgGameObject& gameObject) {

		glm::vec3 rotate{ 0 };
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		// only rotate if rotation is non zero
		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch to +/- ~ 90 degrees to prevent upside down
		gameObject.transform.rotation.x = glm::clamp(
			gameObject.transform.rotation.x, -1.5f, 1.5f);

		// prevent rotation value overflow
		gameObject.transform.rotation.y = glm::mod(
			gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDirection{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDirection{ forwardDirection.z, 0.f, -forwardDirection.x };
		const glm::vec3 upDirection{ 0.f, -1.f, 0.f };

		glm::vec3 moveDirection{ 0.f };
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDirection += forwardDirection;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDirection -= forwardDirection;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDirection += upDirection;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDirection -= upDirection;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDirection += rightDirection;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDirection -= rightDirection;

		// only translate if translation is non zero
		if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon()) {
			gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDirection);
		}
	}

} // namespace SunGlassEngine
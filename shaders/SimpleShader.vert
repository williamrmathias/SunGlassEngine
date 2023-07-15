#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec3 fragNormalWorld;

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projectionViewMatrix;
	vec4 ambientLightColor; // .w = light light intensity
	vec3 lightPosition;
	vec4 lightColor; // .w = light intensity
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
	// PUSH LIMIT = 128 bytes
} push;

void main() {
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projectionViewMatrix * positionWorld;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPositionWorld = positionWorld.xyz;
	fragColor = color;
}
#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;

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

	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

	vec3 directionToLight = ubo.lightPosition - positionWorld.xyz;
	float attenuation = 1.0 / dot(directionToLight, directionToLight); // inverse square law

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	
	// Lambert's Law
	vec3 diffuseLight = lightColor * max(
		dot(normalWorldSpace, normalize(directionToLight)), 0.0);

	// Diffuse + Ambient lighting model
	fragColor = (diffuseLight + ambientLight) * color;
}
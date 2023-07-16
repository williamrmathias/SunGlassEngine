#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
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
	vec3 directionToLight = ubo.lightPosition - fragPositionWorld.xyz;
	float attenuation = 1.0 / dot(directionToLight, directionToLight); // inverse square law

	vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
	vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	
	// Lambert's Law
	vec3 diffuseLight = lightColor * max(
		dot(normalize(fragNormalWorld), normalize(directionToLight)), 0.0);

	// Diffuse + Ambient lighting model
	outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}
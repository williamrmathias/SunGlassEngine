#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

struct PointLight {
    vec4 position; // .w = N/A
    vec4 color; // .w = intensity
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor; // .w = light light intensity
	PointLight pointLights[10]; // 10 = MAX_LIGHTS
    int numLights;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
	// PUSH LIMIT = 128 bytes
} push;

void main() {
	vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 surfaceNormal = normalize(fragNormalWorld);

	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPositionWorld.xyz;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // inverse square law
		float incidentCos = max(
			dot(surfaceNormal, normalize(directionToLight)), 0.0);

		vec3 intensity = light.color.xyz * light.color.w * attenuation;
		diffuseLight += intensity * incidentCos;
	}
	outColor = vec4(diffuseLight * fragColor, 1.0);
}
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
    mat4 invView;
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
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

	vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPositionWorld);

	for (int i = 0; i < ubo.numLights; i++) {
		PointLight light = ubo.pointLights[i];
		vec3 directionToLight = light.position.xyz - fragPositionWorld.xyz;
		float attenuation = 1.0 / dot(directionToLight, directionToLight); // inverse square law

		directionToLight = normalize(directionToLight);

		float incidentCos = max(dot(surfaceNormal, directionToLight), 0.0);

		vec3 intensity = light.color.xyz * light.color.w * attenuation;
		diffuseLight += intensity * incidentCos;

		// specular lighting - Phong-Blinn
		vec3 halfAngle = normalize(directionToLight + viewDirection);
		float blinnTerm = dot(surfaceNormal, halfAngle);
		blinnTerm = clamp(blinnTerm, 0, 1);
		blinnTerm = pow(blinnTerm, 32.0);
		specularLight += intensity * blinnTerm;
	}
	outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}
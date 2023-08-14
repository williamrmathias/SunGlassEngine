#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPositionWorld;
layout(location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

struct PointLight {
    vec4 position; // .w = N/A
    vec4 color; // .w = intensity
};

struct Wave {
    vec2 waveVector; // wave direction * wave number
    float amplitude;
    float phase;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
	mat4 projection;
	mat4 view;
    mat4 invView;
	vec4 ambientLightColor; // .w = light light intensity

	PointLight pointLights[10]; // 10 = MAX_LIGHTS
    Wave waves[3]; // 3 = MAX_WAVES

    int numLights;
    int numWaves;
} ubo;

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
	// PUSH LIMIT = 128 bytes
} push;

vec4 gersterDisplace(vec4 positionWorld, int numWaves) {
    vec2 xz = {0.f, 0.f};
    float y = 0.f;
    for (int i = 0; i < numWaves; i++) {
        float theta = dot(ubo.waves[i].waveVector, positionWorld.xz) - ubo.waves[i].phase;

        xz += normalize(ubo.waves[i].waveVector) * ubo.waves[i].amplitude * sin(theta);
        y += ubo.waves[i].amplitude * cos(theta);
    }

    return vec4(-xz.x, y, xz.y, 0.f);
}

vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.f, 3.f, -1.f));

void main() {
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
	vec3 specularLight = vec3(0.0);
	vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.invView[3].xyz;
	vec3 viewDirection = normalize(cameraPosWorld - fragPositionWorld);

    float incidentCos = max(dot(surfaceNormal, DIRECTION_TO_LIGHT), 0.0);
    diffuseLight += vec3(1.f, 1.f, 1.f) * incidentCos;

    // specular lighting - Phong-Blinn
    vec3 halfAngle = normalize(DIRECTION_TO_LIGHT + viewDirection);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 32.0);
    specularLight += vec3(1.f, 1.f, 1.f) * blinnTerm;

	outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.f);
}
#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPositionWorld;
layout(location = 2) out vec3 fragNormalWorld;

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

void main() {
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    positionWorld += gersterDisplace(positionWorld, ubo.numWaves);
	gl_Position = ubo.projection * ubo.view * positionWorld;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPositionWorld = positionWorld.xyz;
	fragColor = color;
}
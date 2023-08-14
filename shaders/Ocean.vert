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

        xz -= normalize(ubo.waves[i].waveVector) * ubo.waves[i].amplitude * sin(theta);
        y += ubo.waves[i].amplitude * cos(theta);
    }

    return vec4(-xz.x, y, xz.y, 0.f);
}

vec3 gersterNormal(vec4 positionWorld, int numWaves) {
    vec3 alpha = vec3(0.f);
    vec3 beta = vec3(0.f);

    for (int i = 0; i < numWaves; i++) {
        float theta = dot(ubo.waves[i].waveVector, positionWorld.xz) - ubo.waves[i].phase;
        float invWaveNum = 1.f / length(ubo.waves[i].waveVector);

        float ampCosTheta = ubo.waves[i].amplitude * cos(theta);
        float ampSinTheta = ubo.waves[i].amplitude * sin(theta);

        alpha += vec3(ubo.waves[i].waveVector.x * ubo.waves[i].waveVector.x * invWaveNum * ampCosTheta,
                        ubo.waves[i].waveVector.x * ampSinTheta,
                        ubo.waves[i].waveVector.x * ubo.waves[i].waveVector.y * invWaveNum * ampCosTheta);

        beta += vec3(ubo.waves[i].waveVector.x * ubo.waves[i].waveVector.y * invWaveNum * ampCosTheta,
                        ubo.waves[i].waveVector.y * ampSinTheta,
                        ubo.waves[i].waveVector.y * ubo.waves[i].waveVector.y * invWaveNum * ampCosTheta);
    }

    alpha = vec3(1.f - alpha.x, -alpha.y, -alpha.z);
    beta = vec3(-alpha.x, -alpha.y, 1 - alpha.z);
    return normalize(cross(beta, alpha));
}

void main() {
	vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    vec3 normalWorld = gersterNormal(positionWorld, ubo.numWaves);
    positionWorld += gersterDisplace(positionWorld, ubo.numWaves);
	gl_Position = ubo.projection * ubo.view * positionWorld;

	fragNormalWorld = normalWorld;
	fragPositionWorld = positionWorld.xyz;
	fragColor = color;
}
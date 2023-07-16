#version 450

layout (location = 0) in vec2 fragOffset;
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
    vec4 position;
    vec4 color;
    float radius;
} push;

const float M_PI = 3.1415926538;

void main() {
    float distance = sqrt(dot(fragOffset, fragOffset));
    if (distance >= 1.0) discard;
    float distanceCos = 0.5 * (cos(distance * M_PI) + 1.0);
    outColor = vec4(push.color.xyz + distanceCos, distanceCos);
}
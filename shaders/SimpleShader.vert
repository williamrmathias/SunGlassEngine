#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
	mat4 transform; // projection * view * model
	mat4 normalMatrix; // model
	// PUSH LIMIT = 128 bytes
} push;

// using directional lighting in world space
const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));

// using Ambient Lighting Model
const float AMBIENT = 0.02;

void main() {
	// convert position from model space to clip space
	gl_Position = push.transform * vec4(position, 1.0);
	
	// convert normal from model space to world space
	// normal has .w = 0, so only mat3 is needed
	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

	// use Diffuse Lighting Model; Lambert's Law
	// if dot prod is < zero, the face is facing away from the light
	float lightIntensity = AMBIENT + max(
		dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0.0
	);

	fragColor = lightIntensity * color;
}
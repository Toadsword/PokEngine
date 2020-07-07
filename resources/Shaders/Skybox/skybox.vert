#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UboScene {
	mat4 projection;
	mat4 view;
} scene;

layout(binding = 1) uniform UboObject {
	mat4 transform;
	vec4 baseColour;
	vec4 fogColour;
	vec2 fogLimits;
	float blendFactor;
} object;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outUVW;

out gl_PerVertex 
{
	vec4 gl_Position;
};

void main() 
{
	vec3 pos = vec3(inPosition.x, inPosition.y, inPosition.z);
	vec4 worldPosition = object.transform * vec4(pos, 1.0f);

	mat4 viewStatic = mat4(scene.view);
	viewStatic[3][0] = 0.0f;
	viewStatic[3][1] = 0.0f;
	viewStatic[3][2] = 0.0f;
	
	gl_Position = scene.projection * viewStatic * worldPosition;

	outPosition = vec3(viewStatic * worldPosition);
	outUVW = inPosition;
}
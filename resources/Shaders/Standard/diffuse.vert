#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Vertex attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in mat4 inModelMatrix;

layout(binding = 0) uniform UniformScene
{
	mat4 projection;
	mat4 view;
} scene;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main()
{
	vec4 position = vec4(inPosition, 1.0f);
	vec4 normal = vec4(inNormal, 0.0f);

	vec4 worldPosition = inModelMatrix * position;
	mat3 normalMatrix = transpose(inverse(mat3(inModelMatrix)));

	gl_Position = scene.projection * scene.view * worldPosition;

	outPosition = worldPosition.xyz;
	outUV = inUV;
	outNormal = normalMatrix * normalize(normal.xyz);
}


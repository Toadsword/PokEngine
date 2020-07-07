#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform UniformObject
{
	vec4 color;
	float roughness;
	float metallic;
} object;
layout(binding = 2) uniform sampler2D samplerRMA;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outDiffuse;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outMaterial;

#define NORMAL_MAPPING

void main()
{
	vec4 diffuse = object.color;
	vec3 normal = normalize(inNormal);
	vec3 material = vec3(object.roughness, object.roughness, 0.0f);

	//Material mapping
	vec4 textureMaterial = texture(samplerRMA, inUV);
	material.x = textureMaterial.r;
	material.y = textureMaterial.g;
	material.z = textureMaterial.b;
	//Material mapping

	outPosition = vec4(inPosition, 1.0);
	outDiffuse = diffuse;
	outNormal = vec4(normalize(normal), 1.0f);
	outMaterial = vec4(material, 1.0f);
}
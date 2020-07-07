#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform UniformObject
{
	vec4 color;
	float roughness;
	float metallic;
} object;

layout(binding = 2) uniform sampler2D samplerBaseColor;
layout(binding = 3) uniform sampler2D samplerNormal;

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
	vec3 material = vec3(object.roughness, object.metallic, 0.0f);

	//Diffuse mapping
	diffuse *= texture(samplerBaseColor, inUV);
	//Diffuse mapping

	//Normal
	vec3 tangentNormal = texture(samplerNormal, inUV).rgb * 2.0f - 1.0f;
	
	vec3 q1 = dFdx(inPosition);
	vec3 q2 = dFdy(inPosition);
	vec2 st1 = dFdx(inUV);
	vec2 st2 = dFdy(inUV);

	vec3 N = normalize(inNormal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	normal = TBN * tangentNormal;
	//Normal

	outPosition = vec4(inPosition, 1.0);
	outDiffuse = diffuse;
	outNormal = vec4(normalize(normal), 1.0f);
	outMaterial = vec4(material, 1.0f);
}
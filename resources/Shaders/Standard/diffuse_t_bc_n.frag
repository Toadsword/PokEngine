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

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBright;

#define NORMAL_MAPPING

void main()
{
	vec4 diffuse = object.color;
	vec3 normal = normalize(inNormal);
	vec3 material = vec3(0.0f, 0.0f, 0.0f); 

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

	outColor = diffuse;
	
	if(normal != vec3(0.0))
	{
		float brightness = dot(diffuse.xyz, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 5) {
			outBright = diffuse;
		} else {
			outBright = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
	else
	{
		outBright = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
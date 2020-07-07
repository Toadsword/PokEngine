#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform UboObject {
	mat4 transform;

	vec4 color;
} object;

layout(binding = 2) uniform sampler2D samplerColor;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBright;

void main() {
	vec4 diffuse = object.color;
	vec3 normal = normalize(inNormal);
	vec3 material = vec3(0.0f, 0.0f, 0.0f); 

	vec4 color = diffuse * texture(samplerColor, inUV);

	outColor = color;

	float brightness = dot(color.xyz, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.8) {
		outBright = color;
	} else {
		outBright = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
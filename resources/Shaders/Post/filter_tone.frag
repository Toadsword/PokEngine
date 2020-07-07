#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;

layout(binding = 1) uniform sampler2D samplerColor;

layout(location = 0) in vec2 inUV;

const float gamma = 2.2f;

vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1. / gamma));
	return color;
}

void main() {
	vec3 textureColor = texture(samplerColor, inUV).rgb * 2;
	vec3 color = Uncharted2Tonemap(textureColor);
	
	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), vec4(color.r, color.g, color.b, 1.0f));
}
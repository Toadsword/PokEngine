#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0, rgba16) uniform writeonly image2D writeColor;
layout(binding = 1) uniform sampler2D samplerColor;
layout(binding = 2) uniform sampler2D brightColor;

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
	float exposure = 2.2;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1.0 / gamma));
	return color;
}

void main() {
	vec4 hdrColor = texture(brightColor, inUV);      
    vec4 bloomColor = texture(samplerColor, inUV);
    hdrColor += bloomColor; // additive blending
	
	if(hdrColor.a == 1){
		hdrColor = vec4(Uncharted2Tonemap(hdrColor.rgb), 1.0);
	}

	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), vec4(hdrColor.rgb, 1.0));
}
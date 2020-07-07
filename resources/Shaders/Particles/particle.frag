#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 1) uniform sampler2D samplerColor;
layout(binding = 2) uniform sampler2D samplerDepth;

layout(location = 0) in vec2 inCoords1;
layout(location = 1) in vec2 inCoords2;
layout(location = 2) in vec4 inColorOffset;
layout(location = 3) in float inBlendFactor;
layout(location = 4) in float inTransparency;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBright;

float calc_depth( in float z )
{
  return (2.0 * 0.1f) / (1000.0f + 0.1f - z*(1000.0f - 0.1f) );
}

float saturate(float x)
{
  return max(0, min(1, x));
}

void main() {
	vec4 color1 = pow(texture(samplerColor, inCoords1), vec4(2.2));
	vec4 color2 = pow(texture(samplerColor, inCoords2), vec4(2.2));

	outColor = mix(color1, color2, inBlendFactor);
	outColor *= inColorOffset;
	outColor.a *= inTransparency;

	vec2 uv = gl_FragCoord.xy / vec2(1540, 845);
	float z1 = calc_depth(texture(samplerDepth, uv).x);
	float z2 = calc_depth(gl_FragCoord.z);

	float fade = saturate((z1 - z2) * 10000.0f);
	
	outColor.a *= fade;

	float brightness = dot(outColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.0){
		outBright = vec4(outColor.rgb, 1.0);
	} else {
		outBright = vec4(0.0, 0.0, 0.0, 1.0);
	}
}
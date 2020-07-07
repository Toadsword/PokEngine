#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform UniformScene
{
	mat4 view; 

	vec3 cameraPosition; 
	int pointLightCount;

	vec3 directionalLightDirection;
	int spotLightCount;

	vec4 directionalLightColor;
} scene;

struct PointLight
{
	vec4 color;
	vec3 position;
	float radius;
};

layout(binding = 1) buffer BufferPointLights
{
	PointLight pointLights[];
} bufferPointLights;

struct SpotLight
{
	vec4 color;
	vec3 position;
	float range;
	vec3 target;
	float angle;
};

layout(binding = 2) buffer BufferSpotLights
{
	SpotLight spotLights[];
} bufferSpotLights;

layout(binding = 3) uniform sampler2D samplerPosition;
layout(binding = 4) uniform sampler2D samplerDiffuse;
layout(binding = 5) uniform sampler2D samplerNormal;
layout(binding = 6) uniform sampler2D samplerMaterial;
layout(binding = 7) uniform sampler2D samplerBRDF;
layout(binding = 8) uniform samplerCube samplerIrradiance;
layout(binding = 9) uniform samplerCube samplerPrefiltered;

layout(location = 0) in vec2 inUV;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outBright;

const float PI = 3.1415926535897932384626433832795f;

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   
// ----------------------------------------------------------------------------

void main()
{

	vec3 worldPosition = texture(samplerPosition, inUV).rgb;
	vec4 screenPosition = scene.view * vec4(worldPosition, 1.0f);
	
	vec4 diffuse = texture(samplerDiffuse, inUV);
	vec3 normal = texture(samplerNormal, inUV).rgb;
	vec3 material = texture(samplerMaterial, inUV).rgb;

	float metallic = material.g;
	float roughness = material.r;
	
	vec3 N = normalize(normal);
	vec3 V = normalize(scene.cameraPosition - worldPosition);
	vec3 R = reflect(-V, N); 
	
	if(normal != vec3(0.0))
	{
		vec3 F0 = vec3(0.04f);
		F0 = mix(F0, diffuse.rgb, metallic);
		vec3 Lo = vec3(0.0f);

		//Point lights
		for(int i = 0; i < scene.pointLightCount; i++)
		{
			// calculate per-light radiance
			PointLight light = bufferPointLights.pointLights[i];
			vec3 L = normalize(light.position - worldPosition);
			vec3 H = normalize(V + L);
			float Dl = length(light.position.xyz - worldPosition);
			float attenuation = 1.0 / (Dl * Dl);
			float distanceAttenuation = smoothstep(light.radius, 0.0, Dl);
			vec3 radiance = (light.color * attenuation * distanceAttenuation).xyz;

			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, roughness);   
			float G = GeometrySmith(N, V, L, roughness);    
			vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
			vec3 nominator = NDF * G * F;
			float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
			vec3 specular = nominator / denominator;
        
			// kS is equal to Fresnel
			vec3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 kD = vec3(1.0) - kS;
			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			kD *= 1.0 - metallic;	                
            
			// scale light by NdotL
			float NdotL = max(dot(N, L), 0.0);        

			// add to outgoing radiance Lo
			Lo += (kD * diffuse.xyz / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		}

		//Directional light
		{
			// calculate per-light radiance
			vec3 L = scene.directionalLightDirection.xyz;
			vec3 H = normalize(V + L);
			vec3 radiance = scene.directionalLightColor.xyz;

			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, roughness);   
			float G = GeometrySmith(N, V, L, roughness);    
			vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
			vec3 nominator = NDF * G * F;
			float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
			vec3 specular = nominator / denominator;
        
			// kS is equal to Fresnel
			vec3 kS = F;
			// for energy conservation, the diffuse and specular light can't
			// be above 1.0 (unless the surface emits light); to preserve this
			// relationship the diffuse component (kD) should equal 1.0 - kS.
			vec3 kD = vec3(1.0) - kS;
			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			kD *= 1.0 - metallic;	                
            
			// scale light by NdotL
			float NdotL = max(dot(N, L), 0.0);        

			// add to outgoing radiance Lo
			Lo += (kD * diffuse.xyz / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
		}

//		Spot lights
		for(int i = 0; i < scene.spotLightCount; i++)
		{
			SpotLight light = bufferSpotLights.spotLights[i];
			vec3 L = light.position.xyz - worldPosition;
			float Dl = length(L);
			L /= Dl;
			
			float lightCosInnerAngle = cos(radians(light.angle * 0.5));
			float lightCosOuterAngle = cos(radians(light.angle));

			// Direction vector from source to target
			vec3 dir = normalize(light.position.xyz - light.target.xyz);

			// Dual cone spot light with smooth transition between inner and outer angle
			float cosDir = dot(L, dir);
			float spotEffect = smoothstep(lightCosOuterAngle, lightCosInnerAngle, cosDir);
			float heightAttenuation = smoothstep(light.range, 0.0, Dl);
			vec3 radiance = spotEffect * heightAttenuation * light.color.rgb;

			vec3 H = normalize(V + L);

			// Cook-Torrance BRDF
			float NDF = DistributionGGX(N, H, roughness);   
			float G = GeometrySmith(N, V, L, roughness);    
			vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);        
        
			vec3 nominator = NDF * G * F;
			float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
			vec3 specular = nominator / denominator;

			// scale light by NdotL
			float NdotL = max(dot(N, L), 0.0);   

			// kS is equal to Fresnel
			vec3 kS = F;

			// multiply kD by the inverse metalness such that only non-metals 
			// have diffuse lighting, or a linear blend if partly metal (pure metals
			// have no diffuse light).
			vec3 kD = vec3(1.0) - kS;
			kD *= 1.0 - metallic;	

			Lo += (kD * diffuse.xyz / PI + specular) * radiance * NdotL;
		}
				
		// ambient lighting (we now use IBL as the ambient term)
		vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;	  

		vec3 irradiance = texture(samplerIrradiance, N).rgb;
		vec3 diffuse = irradiance * diffuse.xyz;
    
		// sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
		const float MAX_REFLECTION_LOD = 4.0;
		vec3 prefilteredColor = textureLod(samplerPrefiltered, R,  roughness * MAX_REFLECTION_LOD).rgb;    
		vec2 brdf  = texture(samplerBRDF, vec2(max(dot(N, V), 0.0), roughness)).rg;
		vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

//		vec3 ambient = (kD * diffuse + specular) * material.z; //z is the ao
		vec3 ambient = (kD * diffuse + specular) * 0.1f; // only use 90% of the ambiant
    
		vec3 color = ambient + Lo;

		outColor = vec4(color, 1.0);

		float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1) {
			outBright = vec4(color, 1.0);
		} else {
			outBright = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}
	else
	{
		outColor = vec4(diffuse.rgb, 1.0);
		outBright = vec4(0.0, 0.0, 0.0, 0.0);
	}
}
#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0, rgba8) uniform writeonly image2D writeColor;
layout(binding = 1) uniform sampler2D samplerColor;

layout(binding = 2) uniform UniformScene
{
	float time;
} scene;

layout(location = 0) in vec2 inUV;

void main() {
	//Sawtooth function to pulse from centre.
	float offset = (scene.time - floor(scene.time))/scene.time;
	float CurrentTime = (scene.time)*(offset);  
    
	vec3 WaveParams = vec3(10.0, 0.8, 0.1 ); 

    ivec2 originalSize = imageSize(writeColor);
    float ratio = 1;
    
    //Use this if you want to place the centre with the mouse instead
	//Vec2 WaveCentre = vec2( iMouse.xy / iResolution.xy );
       
    vec2 WaveCentre = vec2(0.5, 0.5);
    WaveCentre.y *= ratio; 
   
	vec2 texCoord = gl_FragCoord.xy / originalSize.xy;      
    texCoord.y *= ratio;     
	float Dist = distance(texCoord, WaveCentre);
    
	
	vec4 Color = texture(samplerColor,  inUV);
    
	//Only distort the pixels within the parameter distance from the centre
	if ((Dist <= ((CurrentTime) + (WaveParams.z))) && 
		(Dist >= ((CurrentTime) - (WaveParams.z)))) 
	{
        //The pixel offset distance based on the input parameters
		float Diff = (Dist - CurrentTime); 
		float ScaleDiff = (1.0 - pow(abs(Diff * WaveParams.x), WaveParams.y)); 
		float DiffTime = (Diff  * ScaleDiff);
        
        //The direction of the distortion
		vec2 DiffTexCoord = normalize(texCoord - WaveCentre);         
        
        //Perform the distortion and reduce the effect over time
		texCoord += ((DiffTexCoord * DiffTime) / (CurrentTime * Dist * 40.0));
		Color = texture(samplerColor, texCoord);
        
        //Blow out the color and reduce the effect over time
		Color += (Color * ScaleDiff) / (CurrentTime * Dist * 40.0);
	} 

//	Color = texture(samplerColor, texCoord);
    
	imageStore(writeColor, ivec2(inUV * imageSize(writeColor)), vec4(Color.rgb, 1.0));
}
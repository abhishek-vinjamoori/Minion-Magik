#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;
smooth in vec3 vNormal; 
// output data
out vec4 color;


uniform sampler2D ttemp[2];
uniform vec4 vColor; 


struct SimpleDirectionalLight 
{ 
   vec3 vColor; 
   vec3 vDirection; 
   float fAmbientIntensity; 
}; 

uniform SimpleDirectionalLight sunLight; 


void main()
{
    // Output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices of the triangle
    vec4 texcolor = vec4(texture(ttemp[0],fragColor.xy).rgb,1);
    float fDiffuseIntensity = max(0.0, dot(normalize(vNormal), -sunLight.vDirection)); 
    color = texcolor*vColor*vec4(sunLight.vColor*(sunLight.fAmbientIntensity+fDiffuseIntensity), 1.0); 
}

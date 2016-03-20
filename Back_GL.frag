#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;
in vec3 lightcolor;
in vec3 playerpositionout;

// output data
out vec4 color;
uniform sampler2D ttemp[2];

void main()
{
    // Output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices of the triangle
    
    float dist = length(vec2(lightcolor.x,lightcolor.y));
    //float dist = length(playerpositionout);

    color = vec4(texture(ttemp[0],fragColor.xy).rgb,1);
    color = color*(pow(1-dist,4)+0.2);
    //color = color*(1.0/dist);
}

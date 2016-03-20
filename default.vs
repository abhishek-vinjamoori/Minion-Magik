#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 uv_coords;
layout(location=2)  in vec3 norm;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 light_pos;
uniform vec3 torch_pos;

out vec2 uv;
out vec3 pos_world;
out vec3 norm_cam;
out vec3 eye_cam;
out vec3 light_cam;
out vec3 torch_cam;

void main(){
	uv=uv_coords;
	gl_Position=MVP*vec4(position,1);
	pos_world=(M*vec4(position,1)).xyz;
	norm_cam=(V*M*vec4(norm,0)).xyz;
	eye_cam=vec3(0,0,0)-(V*M*vec4(position,1)).xyz;
	light_cam=(V*vec4(light_pos,1)).xyz+eye_cam;
	torch_cam=(V*vec4(torch_pos,1)).xyz+eye_cam;
}

#version 330 core

in vec2 uv;
in vec3 pos_world;
in vec3 norm_cam;
in vec3 eye_cam;
in vec3 light_cam;
in vec3 torch_cam;

uniform sampler2D tex;
uniform vec3 light_pos;
uniform vec3 torch_pos;
uniform vec3 torch_direction;
uniform vec3 torch_color;
uniform float torch_power;
uniform vec3 emit_color;
uniform vec3 spec_color;

out vec4 final_color;

void main(){
	vec3 light_color=vec3(1,1,1);
	float light_power=30;
	
	vec3 color;
	vec4 texture_element=texture(tex,uv).rgba;
	float color_a=texture_element.a;
	vec3 diff_color=texture_element.rgb;
	vec3 amb_color=vec3(0.1,0.1,0.1)*diff_color;
	vec3 spec_color=spec_color;

	float dist=length(light_pos-pos_world);
	vec3 n=normalize(norm_cam);
	vec3 l=normalize(light_cam);
	float cos_theta=clamp(dot(n,l),0,1);

	vec3 e=normalize(eye_cam);
	vec3 r=reflect(-l,n);
	float cos_alpha=clamp(dot(e,r),0,1);

	color=amb_color+diff_color*light_color*light_power*cos_theta/(dist*dist)+spec_color*light_color*light_power*pow(cos_alpha,8)/(dist*dist);
	float angle=dot(normalize(torch_direction),normalize(pos_world-torch_pos));
	if(angle>0.8){
		light_color=torch_color;
		light_power=torch_power;
		vec3 diff_color=texture(tex,uv).rgb;
		vec3 spec_color=spec_color;

		dist=length(torch_pos-pos_world);
		n=normalize(norm_cam);
		l=normalize(torch_cam);
		cos_theta=clamp(dot(n,l),0,1);

		e=normalize(eye_cam);
		r=reflect(-l,n);
		cos_alpha=clamp(dot(e,r),0,1);
	
		color+=diff_color*light_color*light_power*cos_theta/(dist*dist)+spec_color*light_color*light_power*pow(cos_alpha,8)/(dist*dist);
	}
	else if(angle>0.6){
		light_color=torch_color;
		light_power=torch_power;
		vec3 diff_color=texture(tex,uv).rgb;
		vec3 spec_color=spec_color;

		dist=length(torch_pos-pos_world);
		n=normalize(norm_cam);
		l=normalize(torch_cam);
		cos_theta=clamp(dot(n,l),0,1);

		e=normalize(eye_cam);
		r=reflect(-l,n);
		cos_alpha=clamp(dot(e,r),0,1);
	
		color+=(diff_color*light_color*light_power*cos_theta/(dist*dist)+spec_color*light_color*light_power*pow(cos_alpha,8)/(dist*dist))*(angle-0.6)/0.2;
	}
	color=color+emit_color;
	final_color=vec4(color,color_a);
}

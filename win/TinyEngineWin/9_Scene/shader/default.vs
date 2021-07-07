#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec4 in_Color;

uniform mat4 model;
uniform mat4 vp;
uniform mat4 dbvp;

out vec4 ex_Color;
out vec3 ex_Normal;
out vec3 ex_Model; 	//Model Space
out vec4 ex_Shadow;	//Shadow Space
out vec4 ex_Frag;

void main(void) {
	ex_Model = (model * vec4(in_Position, 1.0f)).xyz;
	ex_Normal = in_Normal;
	ex_Shadow = dbvp * vec4(ex_Model, 1.0f);
	gl_Position = vp * vec4(ex_Model, 1.0f);
	ex_Color = in_Color;
}

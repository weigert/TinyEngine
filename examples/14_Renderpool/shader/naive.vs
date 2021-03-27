#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec4 in_Color;

out vec3 ex_FragPos;
out vec4 ex_Color;

uniform mat4 vp;

void main(void){
	gl_Position = vp*vec4(in_Position, 1.0f);
	ex_Color = in_Color;
}

#version 330

in vec3 in_Position;
in vec3 in_Normal;

uniform mat4 model;
uniform mat4 vp;

out vec4 ex_Color;

void main(void) {

	gl_Position = vp * model * vec4(in_Position, 1.0f);
	ex_Color = vec4(0.2,0.2,0.2,1);

}

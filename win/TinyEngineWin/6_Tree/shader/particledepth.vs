#version 330 core
in vec3 in_Quad;
in vec2 in_Tex;
in mat4 in_Model;

out vec2 ex_Tex;

uniform mat4 dvp;

void main(void) {
	ex_Tex = in_Tex;
	gl_Position = dvp * in_Model * vec4(in_Quad, 1.0f);
}

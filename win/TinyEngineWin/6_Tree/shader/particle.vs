#version 130

in vec3 in_Quad;
in vec2 in_Tex;
in mat4 in_Model;

uniform mat4 projectionCamera;
uniform mat4 dbvp;

out vec2 ex_Tex;
out vec3 ex_Color;
out vec4 ex_Shadow;

void main(void) {
	//Pass Texture Coordinates
	ex_Tex = in_Tex;

	//Actual Position in Space
	gl_Position = projectionCamera * in_Model * vec4(in_Quad, 1.0f);
	ex_Shadow = dbvp * in_Model * vec4(in_Quad, 1.0f);
}

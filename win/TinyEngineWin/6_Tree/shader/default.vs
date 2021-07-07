#version 130

in vec3 in_Position;
in vec3 in_Normal;
in vec4 in_Color;

uniform mat4 model;
uniform mat4 projectionCamera;
uniform mat4 dbvp;

out vec4 ex_Color;
out vec3 ex_Normal;
out vec3 ex_FragPos;
out vec4 ex_Shadow;


void main(void) {
	//Fragment Position in Model Space
	ex_FragPos = (model * vec4(in_Position, 1.0f)).xyz;
	ex_Normal = in_Normal;	//Pass Normal
	ex_Shadow = dbvp* vec4(ex_FragPos, 1.0f);

	//Fragment in Screen Space
	gl_Position = projectionCamera * vec4(ex_FragPos, 1.0f);

	//Color from Normal Vector
	ex_Color = in_Color;//vec4(normalize(in_Normal), 1.0);
}

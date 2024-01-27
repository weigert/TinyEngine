#version 430 core

layout(location = 0) in vec4 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in mat4 in_Model;

uniform mat4 proj;
uniform mat4 view;

out vec4 ex_Position;
out vec3 ex_Normal;

void main(void) {

	const vec4 v_Position = view * in_Model * in_Position;
	ex_Position = v_Position;
	ex_Normal = transpose(inverse(mat3(view * in_Model))) * normalize(in_Normal);
	gl_Position = proj*v_Position;

}

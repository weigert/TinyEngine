#version 330 core

layout(location = 0) in vec3 in_Position;

uniform mat4 vp;
out vec3 ex_Normal;

void main() {

	gl_Position = vp * vec4(in_Position, 1.0f);
	ex_Normal = normalize(in_Position);

}

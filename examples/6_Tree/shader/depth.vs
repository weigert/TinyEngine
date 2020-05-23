#version 330 core
in vec3 in_Position;
uniform mat4 model;
uniform mat4 dvp;

void main(void) {
	// Set the position to the one defined in our vertex array
	gl_Position = dvp * model * vec4(in_Position, 1.0f);
}

#version 330 core

in vec3 ex_Color;
out vec4 fragColor;

void main(void) {
  fragColor = vec4(ex_Color, 0.2);
}

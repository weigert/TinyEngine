#version 460 core

in vec3 ex_Color;
out vec4 fragColor;

void main() {
  fragColor = vec4(ex_Color, 1.0f);
}

#version 330

in vec4 ex_Color;
out vec4 fragColor;

void main(void) {
  fragColor = vec4(ex_Color.xyz, 1.0);
}

#version 330

in vec4 ex_Color;
out vec4 fragColor;

void main(void) {
  fragColor = ex_Color; //Pass Color to Fragment
}

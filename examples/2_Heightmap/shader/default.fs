#version 130

in vec4 ex_Color;
in vec3 ex_Normal;
in vec3 ex_FragPos;

out vec4 fragColor;

void main(void) {
  fragColor = ex_Color; //Pass Color to Fragment
}

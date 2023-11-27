#version 330 core

in vec3 ex_Tex;
out vec4 fragColor;

void main(){
  fragColor = vec4(ex_Tex, 1);
}

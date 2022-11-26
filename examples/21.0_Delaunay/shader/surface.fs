#version 430 core

in float ex_Depth;
in vec3 ex_Color;

out vec4 fragColor;

void main(){

  fragColor = vec4(ex_Color, 1);

}

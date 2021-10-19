#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform vec3 color;

void main(){
  fragColor = vec4(color, 1.0f);
}

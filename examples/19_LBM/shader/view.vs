#version 460 core

in vec2 in_Quad;
in vec2 in_Tex;
out vec2 ex_Tex;

void main(){

  ex_Tex = vec2(in_Tex.x, 1-in_Tex.y);
  gl_Position = vec4(in_Quad, 1.0, 1.0);

}

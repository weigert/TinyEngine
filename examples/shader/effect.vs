#version 130
in vec2 in_Quad;
in vec2 in_Tex;
out vec2 ex_Tex;

void main(){
  ex_Tex = in_Tex;
  gl_Position = vec4(in_Quad, -1.0, 1.0);
}

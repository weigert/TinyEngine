#version 330
in vec2 in_Quad;
in vec2 in_Tex;
out vec2 ex_Tex;

//Position the Billboard in space!
uniform mat4 model;

void main(){
  ex_Tex = vec2(in_Tex.x, 1.0-in_Tex.y);
  gl_Position = model*vec4(in_Quad, 1.0, 1.0);
}

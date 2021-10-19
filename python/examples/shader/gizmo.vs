#version 330 core

in vec3 in_Quad;
in vec3 in_Tex;
out vec3 ex_Tex;

//Position the Billboard in space!
//uniform mat4 model;
uniform mat4 vp;

void main(){
  ex_Tex = in_Tex;
  gl_Position = vp*vec4(in_Quad, 1.0);
  gl_Position.w = 1.0f;
}

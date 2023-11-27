#version 460 core

layout(location = 0) in vec4 in_Pos;

uniform mat4 vp;

void main() {
  gl_Position = vp*in_Pos;
}

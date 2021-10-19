#version 330 core

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Color;

uniform mat4 vp;

out vec3 ex_Color;

void main() {
  gl_Position = vp*vec4(in_Position, 1.0f);
  ex_Color = in_Color;
}

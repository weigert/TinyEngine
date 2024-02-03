#version 430 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gColor;

in vec4 ex_Position;
in vec3 ex_Normal;

uniform vec3 color;

void main(){

  gPosition = ex_Position;
  gNormal = vec4(normalize(ex_Normal), 1);
  gColor = vec4(1);

}
#version 460 core

layout (location = 0) in vec4 in_Position;

out VS_OUT {
  vec4 position;
} vs_out;

uniform mat4 vp;
uniform mat4 model;

void main(){

  vs_out.position = in_Position;

}

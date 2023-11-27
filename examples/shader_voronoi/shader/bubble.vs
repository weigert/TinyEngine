#version 430 core
layout (location = 0) in vec2 in_Quad;
layout (location = 1) in vec2 in_Tex;
layout (location = 2) in vec2 in_Centroid;

out vec2 ex_Tex;

uniform mat4 model;

void main(){
  ex_Tex = in_Tex;
  gl_Position = model*vec4(in_Quad, 1.0, 1.0);
}

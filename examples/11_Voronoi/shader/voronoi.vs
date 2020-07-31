#version 330 core
layout (location = 0) in vec2 in_Quad;
layout (location = 1) in vec2 in_Tex;
layout (location = 2) in vec2 in_Centroid;
layout (location = 3) in vec3 in_Color;

out vec2 ex_Tex;
out vec2 ex_Centroid;
out vec3 ex_Color;

uniform mat4 model;

void main(){
  ex_Tex = in_Tex;
  ex_Centroid = in_Centroid;
  ex_Color = in_Color;
  gl_Position = model*vec4(in_Quad, -1.0, 1.0);
}

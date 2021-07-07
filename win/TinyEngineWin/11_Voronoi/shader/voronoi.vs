#version 330 core
layout (location = 0) in vec2 in_Quad;
layout (location = 2) in vec2 in_Centroid;

out vec2 ex_Quad;
flat out vec3 ex_Color;
out vec2 ex_Centroid;

uniform float R;

vec3 color(int i){
  float r = ((i >>  0) & 0xff)/255.0f;
  float g = ((i >>  8) & 0xff)/255.0f;
  float b = ((i >> 16) & 0xff)/255.0f;
  return vec3(r,g,b);
}

void main(){
  ex_Centroid = in_Centroid;
  ex_Color = color(gl_InstanceID);
  ex_Quad =  R*in_Quad+in_Centroid;
  gl_Position = vec4(ex_Quad, 0.0, 1.0);
}

#version 330 core
layout (location = 0) in vec2 in_Quad;
layout (location = 2) in vec2 in_Centroid;

out vec2 ex_Quad;
flat out vec3 ex_Color;
out vec2 ex_Centroid;

uniform float R;
uniform int NCOLOR;

vec3 color(int i){
  int Z = (i%NCOLOR);
  int Y = ((i/NCOLOR)%NCOLOR);
  int X = ((i/(NCOLOR*NCOLOR))%NCOLOR);
  return vec3(X, Y, Z)/vec3(NCOLOR-1.0f);
}

void main(){
  ex_Centroid = in_Centroid;
  ex_Color = color(gl_InstanceID);
  ex_Quad =  R*in_Quad+in_Centroid;
  gl_Position = vec4(ex_Quad, 0.0, 1.0);
}

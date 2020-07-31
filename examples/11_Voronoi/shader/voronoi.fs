#version 330 core

in vec2 ex_Quad;
in vec2 ex_Centroid;
flat in vec3 ex_Color;
out vec4 fragColor;

uniform float R;

void main(){
  gl_FragDepth = length(ex_Quad-ex_Centroid);
  if(gl_FragDepth > R) discard; //Make them Round
  fragColor = vec4(ex_Color, 1.0);
}

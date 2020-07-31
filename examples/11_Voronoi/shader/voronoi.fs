#version 330 core

in vec2 ex_Tex;
in vec2 ex_Centroid;
in vec3 ex_Color;

out vec4 fragColor;

uniform vec2 center;
uniform vec3 color;
uniform float r;
vec3 black = vec3(0);

void main(){
  vec2 p = (ex_Tex*2-vec2(1.0));
  gl_FragDepth = length(p-ex_Centroid);
  if(gl_FragDepth > r) discard;
  fragColor = vec4(ex_Color, 1.0);
}

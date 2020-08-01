#version 330 core

in vec2 ex_Quad;
in vec2 ex_Centroid;
flat in vec3 ex_Color;
out vec4 fragColor;

uniform float R;

uniform bool drawcenter;
uniform int style;

void main(){
  gl_FragDepth = length(ex_Quad-ex_Centroid);
  if(gl_FragDepth > R) discard; //Make them Round

  if(style == 1)
    fragColor = vec4(vec3(gl_FragDepth/R), 1.0);
  else
    fragColor = vec4(ex_Color, 1.0);

  if(gl_FragDepth < 0.1*R && drawcenter)
    fragColor = vec4(1.0);
}

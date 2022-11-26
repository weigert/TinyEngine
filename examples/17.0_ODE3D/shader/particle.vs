#version 460 core

layout(location = 0) in vec4 in_Pos;
layout(location = 1) in vec4 in_Col;

uniform mat4 vp;
uniform float op;

out vec4 ex_Col;

const vec3 colA = vec3(1,0,0);
const vec3 colB = vec3(0,0,1);

void main() {

  float v = 0.0005*length(in_Col.xyz);

  ex_Col = mix(vec4(colA,0.8),vec4(colB,0.8), v);
//  ex_Col = vec4(1.0f-normalize(in_Col.xyz), 0.3);


  gl_Position = vp*in_Pos;
}

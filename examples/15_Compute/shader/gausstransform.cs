#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) readonly buffer pointsetA {
  vec4 pA[];
};

layout (std430, binding = 1) readonly buffer pointsetB {
  vec4 pB[];
};

layout (std430, binding = 2) buffer probability {
  float P[];
};

uniform int N;
uniform int M;
uniform float var;
uniform mat4 T;

void main() {

  //uint index = gl_GlobalInvocationID.y*N+gl_GlobalInvocationID.x;
  uint index = gl_GlobalInvocationID.x*N+gl_GlobalInvocationID.y;
  vec4 d = pA[gl_GlobalInvocationID.x] - T*pB[gl_GlobalInvocationID.y];
  P[index] = exp(-(0.5f/var)*dot(d,d));

};

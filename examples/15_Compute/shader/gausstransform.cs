#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) readonly buffer position {
  vec4 pos[];
};

layout (std430, binding = 2) writeonly buffer probability {
  float prob[];
};

uniform int N;
uniform int M;
uniform float var;
uniform mat4 T;

uniform int N0;
uniform int M0;

void main() {

  const uint index = gl_GlobalInvocationID.x*M+gl_GlobalInvocationID.y;
  const vec4 d = pos[N0+gl_GlobalInvocationID.x] - T*pos[M0+gl_GlobalInvocationID.y];
  prob[index] = exp(-(0.5f/var)*dot(d,d));

};

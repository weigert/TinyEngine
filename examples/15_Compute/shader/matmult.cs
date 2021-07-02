#version 460 core

layout(local_size_x = 1, local_size_y = 1024) in;

layout (std430, binding = 0) readonly buffer matrixA {
  float A[];
};

layout (std430, binding = 1) readonly buffer matrixB {
  float B[];
};

layout (std430, binding = 2) writeonly buffer result {
  float R[];
};

uniform int N;
uniform int K;
uniform int M;

void main() {

  const uint index = gl_GlobalInvocationID.x*M+gl_GlobalInvocationID.y;

  float r = 0.0f;
  for(int k = 0; k < K; k++)
    r += A[gl_GlobalInvocationID.x*K+k]*B[k*M+gl_GlobalInvocationID.y];

  R[index] = r;

};

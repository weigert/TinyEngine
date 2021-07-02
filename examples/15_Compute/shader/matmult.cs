#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) readonly buffer matrixA {
  float A[];
};

layout (std430, binding = 1) readonly buffer matrixB {
  float B[];
};

layout (std430, binding = 2) buffer result {
  float R[];
};

uniform int N;
uniform int K;
uniform int M;

void main() {

  const uint index = gl_GlobalInvocationID.x*M+gl_GlobalInvocationID.y;

  float r = 0.0f;
  for(int i = 0; i < K; i++){
    r += A[gl_GlobalInvocationID.x*K+i]*B[i*M+gl_GlobalInvocationID.y];
  }
  
  R[index] = r;

};

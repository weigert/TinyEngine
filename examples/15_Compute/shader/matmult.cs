#version 460 core

//layout(local_size_x = 32, local_size_y = 1) in;
layout(local_size_x = 1, local_size_y = 4) in;

layout (std430, binding = 0) readonly buffer matrixA {
  float A[];
};

layout (std430, binding = 1) readonly buffer matrixB {
  float B[];
};

layout (std430, binding = 2) writeonly buffer result {
  float R[];
};

//uniform int N;
//uniform int K;
//uniform int M;

const int N = 4096;
const int K = 4096;
const int M = 4;

const uint BS = 16;

void main() {

  //Batched Results
  float r[BS];
  for(int i = 0; i < BS; i++)
    r[i] = 0.0f;

  //Iterate over Multiply Direction
  const uint x = gl_GlobalInvocationID.x*BS;

  for(int k = 0; k < K; k++){

    //Current Element of B
    const float By = B[k*M+gl_GlobalInvocationID.y];

    //Accumulate into batched results
    for(int j = 0; j < BS; j++)
      r[j] += A[(x+j)*K+k]*By;

  }

  for(int i = 0; i < BS; i++)
    R[(x+i)*M+gl_GlobalInvocationID.y] = r[i];

};

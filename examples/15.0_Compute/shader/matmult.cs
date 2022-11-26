#version 460 core

layout (std430, binding = 0) readonly buffer matrixA {
  float A[];
};

layout (std430, binding = 1) readonly buffer matrixB {
  float B[];
};

layout (std430, binding = 2) writeonly buffer result {
  float R[];
};

/*
layout(local_size_x = 256, local_size_y = 1) in;

uniform int N;
uniform int K;
uniform int M;

const uint BS = 16;

void main() {

  //Batched Results
  float r[4*BS];
  for(int i = 0; i < 4*BS; i++)
    r[i] = 0.0f;

  for(int b = 0; b < BS; b++){ //Iterate over the Row in R
    const uint x = (gl_GlobalInvocationID.x*BS+b)*K;
    for(int k = 0; k < K; k++){   //Iterate over the Row in A
      float Ay = A[x+k];
      for(int j = 0; j < 4; j++){ //Iterate over the Row in B
        r[b*4+j] += Ay * B[k*4+j];
      }
    }
  }

  for(int b = 0; b < BS; b++)   //Iterate over the Row in R
  for(int j = 0; j < 4; j++)    //Iterate over Row in B
    R[(gl_GlobalInvocationID.x*BS+b)*M+j] = r[b*4+j];

};
*/

layout(local_size_x = 1, local_size_y = 1024) in;

uniform int N;
uniform int K;
uniform int M;

const uint BS = 16;

void main() {

  //Batched Results
  float r[BS];
  for(int i = 0; i < BS; i++)
    r[i] = 0.0f;

  //Iterate over Multiply Direction
  const uint x = gl_GlobalInvocationID.x*BS;

  for(int k = 0; k < K; k++){
    float By = B[k*M+gl_LocalInvocationID.y];
    for(int j = 0; j < BS; j++){
      r[j] += A[(x+j)*K+k]*By;
    }
  }

  for(int i = 0; i < BS; i++)
    R[(x+i)*M+gl_LocalInvocationID.y] = r[i];

};

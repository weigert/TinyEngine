#version 460 core

layout(local_size_x = 1024, local_size_y = 1) in;

layout (std430, binding = 0) buffer buffA {
  int A[];
};

layout (std430, binding = 1) buffer buffB {
  int B[];
};


uniform bool flip;
uniform int stride;

void main(){

  uint index = gl_GlobalInvocationID.x;

//  A[index] += A[index + stride];
//  if(!flip) B[index] = A[2*index+0] + A[2*index+1];
//  else A[index] = B[2*index+0] + B[2*index+1];

  if(!flip) B[index] = A[index] + A[index+stride];
  else A[index] = B[index] + B[index+stride];

};

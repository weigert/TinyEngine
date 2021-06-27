#version 460 core

layout(local_size_x = 1024, local_size_y = 1) in;

layout (std430, binding = 0) buffer buff {
  float A[];
};

uniform int K;
uniform int stride;
uniform int rest;

void main(){

  uint index = gl_GlobalInvocationID.x;
  if(index > stride) return;           //We only operate on the elements within the stride

  float t = 0.0f;
  for(int i = 0; i < K; i++)
    t += A[index + i*stride];
  A[index] = t;

  if(index == 0)
  for(int i = 0; i < rest; i++)
    A[index] += A[K*stride+i];

};

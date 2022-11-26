#version 460 core

layout(local_size_x = 1024) in;

buffer buff {
  float A[];
};

uniform int K;
uniform int stride;
uniform int rest;

void main(){

  const uint index = gl_GlobalInvocationID.x;

  if(index > stride)                  //Only Accumulate within Stride
    return;

  if(stride > 0)                      //Accumulate with Stride
  for(int i = 1; i < K; i++)
    A[index] += A[index + i*stride];

  if(index > 0)                       //Accumulate Remainder at 0
    return;

  if(stride > 0)                      //Remainder at End
  for(int i = 0; i < rest; i++)
    A[0] += A[K*stride+i];

  else
  for(int i = 1; i < rest; i++)       //Remainder at Beginning
    A[0] += A[K*stride+i];

};

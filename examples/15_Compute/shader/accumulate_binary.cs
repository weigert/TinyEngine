#version 460 core

layout(local_size_x = 1024, local_size_y = 1) in;

layout (std430, binding = 0) buffer buff {
  float A[];
};

uniform int stride;

void main(){

  uint index = gl_GlobalInvocationID.x;
  A[index] += A[index + stride];

};

#version 460 core

layout(local_size_x = 1) in;

layout (std430, binding = 0) buffer buff {
  float b[];
};

void main(){

  b[gl_GlobalInvocationID.x] += 1.0f;

};

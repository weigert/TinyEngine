#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) buffer position {
  vec4 p[];
};

layout (std430, binding = 1) readonly buffer velocity {
  vec4 v[];
};

layout (std430, binding = 2) readonly buffer mass {
  float m[];
};

uniform int size;

const float dt = 1E-5;
const float G = 10.0f;

void main() {

  const uint index = gl_GlobalInvocationID.x*64+gl_GlobalInvocationID.y;

  if(index >= size) 
    return;

  p[index] += dt*v[index];

};
#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) buffer vertices {
  vec4 p[];
};

layout (std430, binding = 1) buffer velocity {
  vec4 v[];
};

layout (std430, binding = 2) readonly buffer force {
  vec4 f[];
};

#include common.cs
#include timestep.cs

void main() {

  const uint index = gl_GlobalInvocationID.x*res + gl_GlobalInvocationID.y;
  if(index >= size) 
    return;

  if(t == 0)
    leapfrog(index);
  else {
    verlet(index);
  }

  //symplectic(index);

};

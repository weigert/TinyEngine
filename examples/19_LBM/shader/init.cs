#version 460 core

layout(local_size_x = 25, local_size_y = 25) in;

#include lbm.cs

void main(){

  uint ind = gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y;

  // Initialize the Boundary Condition Array

  B[ind] = 0.0;
  if(length(gl_GlobalInvocationID.xy - vec2(NX/2, NY)/2) < 10)
    B[ind] = 1.0;

  // Initialize the Boltzmann Distribution

  const vec2 init_velocity = vec2(0.2, 0);
  const float init_density = 1.0;
  for(int q = 0; q < Q; q++)
    F[ind*Q + q] = equilibrium(q, init_density, init_velocity);

}

#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

#include lbm.cs

void main(){

  uint ind = gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y;

  // Initialize the Boltzmann Distribution

  const vec2 init_velocity = vec2(0.2, 0);
  const float init_density = 1.0;
  for(int q = 0; q < Q; q++)
    F[ind*Q + q] = equilibrium(q, init_density, init_velocity);

}

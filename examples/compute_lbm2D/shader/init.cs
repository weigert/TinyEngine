#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

#include lbm.cs

void main(){

  uint ind = gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y;

  // Initialize the Boltzmann Distribution

  for(int q = 0; q < Q; q++)
    F[q*NX*NY + ind] = equilibrium(q, init_density, init_velocity);

}

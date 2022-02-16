/*
    Lattice Boltzmann Method Helper Compute Shader File
    Contains almost all relevant methods for the LBM!
*/

// Main Buffers and Parameters

layout (std430, binding = 0) buffer f {
  float F[];
};

layout (std430, binding = 1) buffer fprop {
  float FPROP[];
};

layout (std430, binding = 2) buffer b {
  float B[];
};

// Size of Domain

uniform int NX = 1;
uniform int NY = 1;

// Velocity Set

const int Q = 9;
const float w[Q] = {4.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/9.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0};
const int cp[Q] = {0, 3, 4, 1, 2, 7, 8, 5, 6};
const ivec2 c[Q] = {
  ivec2(0, 0),
  ivec2(1, 0),
  ivec2(0, 1),
  ivec2(-1, 0),
  ivec2(0, -1),
  ivec2(1, 1),
  ivec2(-1, 1),
  ivec2(-1, -1),
  ivec2(1, -1)
};

const float cs = 1.0/sqrt(3.0);
const float cs2 = 1.0/cs/cs;
const float cs4 = 1.0/cs/cs/cs/cs;

// Compute the Equilibrium Boltzmann Distribution

float equilibrium(int q, float rho, vec2 v){

  float eq = 0.0;
  eq += w[q]*rho;
  eq += w[q]*rho*(dot(v, c[q]))*cs2;
  eq += w[q]*rho*(dot(v, c[q])*dot(v, c[q]))*0.5*cs4;
  eq -= w[q]*rho*(dot(v, v))*0.5*cs2;
  return eq;

}

// Compute the Density at a Position

float getRho(uint index){
  float rho = 0.0;
  for(int q = 0; q < 9; q++)
    rho += F[index*Q + q];
  return rho;
}

// Compute the Momentum at a Position

vec2 getV(uint index){

  vec2 v = vec2(0);
  for(int q = 0; q < 9; q++)
    v += F[index*Q + q]*c[q];
  return v;

  /*
  vec2 v;
  v.x = F[index*Q + 1] + F[index*Q + 5] + F[index*Q + 8] - F[index*Q + 3] - F[index*Q + 6] - F[index*Q + 7];
  v.y = F[index*Q + 2] + F[index*Q + 5] + F[index*Q + 6] - F[index*Q + 4] - F[index*Q + 7] - F[index*Q + 8];
  return v;
  */

}

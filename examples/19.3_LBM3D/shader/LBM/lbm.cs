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
uniform int NZ = 1;

uniform float t = 0.0f;
vec3 force = 0.1*vec3(cos(t), 0, sin(t));
//vec3 force = 0.1*vec3(1, 0, 0);//vec3(cos(t), 0, sin(t));

// Velocity Set

/*

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

*/

const int Q = 19;

const float w[Q] = {
  1.0/3.0,
  1.0/18.0, 1.0/18.0, 1.0/18.0, 1.0/18.0, 1.0/18.0, 1.0/18.0,
  1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0,
  1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0, 1.0/36.0
};

const ivec3 c[Q] = {
  ivec3( 0,  0,  0),
  ivec3( 1,  0,  0), ivec3(-1,  0,  0),
  ivec3( 0,  1,  0), ivec3( 0, -1,  0),
  ivec3( 0,  0,  1), ivec3( 0,  0, -1),
  ivec3( 1,  1,  0), ivec3(-1, -1,  0),
  ivec3( 1,  0,  1), ivec3(-1,  0, -1),
  ivec3( 0,  1,  1), ivec3( 0, -1, -1),
  ivec3( 1, -1,  0), ivec3(-1,  1,  0),
  ivec3( 1,  0, -1), ivec3(-1,  0,  1),
  ivec3( 0,  1, -1), ivec3( 0, -1,  1)
};

const int cp[Q] = {
  0,
  2, 1, 4, 3, 6, 5,
  8, 7, 10, 9, 12, 11,
  14, 13, 16, 15, 18, 17
};

const float cs = 1.0/sqrt(3.0);
const float cs2 = 1.0/cs/cs;
const float cs4 = 1.0/cs/cs/cs/cs;

// Compute the Equilibrium Boltzmann Distribution

float equilibrium(int q, float rho, vec3 v){

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
  for(int q = 0; q < Q; q++)
    rho += F[index*Q + q];
  return rho;

}

// Compute the Momentum at a Position

vec3 getV(uint index){

  vec3 v = vec3(0);
  for(int q = 0; q < Q; q++)
    v += F[index*Q + q]*c[q];
  return v;

}

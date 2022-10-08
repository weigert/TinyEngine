#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

#include lbm.cs

layout (std430, binding = 3) buffer rho {
  float RHO[];
};

layout (std430, binding = 4) buffer v {
  vec2 V[];
};

const float tau = 0.6;
const float dt = 1.0;

void main(){

  const uint ind = gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y;

  const float _rho = getRho(ind);
  const vec2 _v = getV(ind)/_rho;

  RHO[ind] = _rho;
  V[ind] = _v;


  // TRT Method

  const float omega_plus = 1.0/tau;
  const float lambda = 0.25;
  const float omega_minus = 1.0/(lambda/(1.0/omega_plus-0.5)+0.5);

  float feqq[9];
  for(int q = 0; q < Q; q++)
    feqq[q] = equilibrium(q, _rho, _v);

  for(int q = 0; q < Q; q++){

    // Compute Equilibrium Distribution (TRT)
    float f_plus, f_minus, feq_plus, feq_minus;

    f_plus  = 0.5*(F[q*NX*NY + ind] + F[cp[q]*NX*NY + ind]);
    f_minus = 0.5*(F[q*NX*NY + ind] - F[cp[q]*NX*NY + ind]);

    feq_plus = 0.5*(feqq[q] + feqq[cp[q]]);
    feq_minus = 0.5*(feqq[q] - feqq[cp[q]]);

    FPROP[q*NX*NY + ind] = F[q*NX*NY + ind] - omega_plus*(f_plus - feq_plus) - omega_minus*(f_minus - feq_minus);

  }

  /*


  // BGK Method

  for(int q = 0; q < Q; q++){
    FPROP[q*NX*NY + ind] = (1.0 - dt/tau)*F[q*NX*NY + ind] + dt/tau*equilibrium(q, _rho, _v);
  }

  */



}

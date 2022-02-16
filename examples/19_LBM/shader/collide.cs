#version 460 core

layout(local_size_x = 25, local_size_y = 25) in;

#include lbm.cs

layout (std430, binding = 3) buffer rho {
  float RHO[];
};

layout (std430, binding = 4) buffer v {
  vec2 V[];
};

const float tau = 0.57;
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

  for(int q = 0; q < Q; q++){

    // Compute Equilibrium Distribution (TRT)

    float f_plus, f_minus, feq_plus, feq_minus;

    f_plus  = 0.5*(F[ind*Q + q] + F[ind*Q + cp[q]]);
    f_minus = 0.5*(F[ind*Q + q] - F[ind*Q + cp[q]]);

    float feq    = equilibrium(q, _rho, _v);
    float feq_cp =  equilibrium(cp[q], _rho, _v);

    feq_plus = 0.5*(feq + feq_cp);
    feq_minus = 0.5*(feq - feq_cp);

    FPROP[ind*Q + q] = F[ind*Q + q] - omega_plus*(f_plus - feq_plus) - omega_minus*(f_minus - feq_minus);

  }

  // BGK Method

  /*

  for(int q = 0; q < Q; q++){
    FPROP[ind*Q + q] = (1.0 - dt/tau)*F[ind*Q + q] + dt/tau*equilibrium(q, _rho, _v);
  }

  */

}

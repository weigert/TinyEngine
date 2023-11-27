#version 460 core

layout(local_size_x = 16, local_size_y = 1, local_size_z = 16) in;

#include lbm.cs

layout (std430, binding = 3) buffer rho {
  float RHO[];
};

layout (std430, binding = 4) buffer v {
  vec4 V[];
};

const float tau = 0.56;
const float dt = 1.0;

void main(){

  const uint ind = (gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y)*NZ + gl_GlobalInvocationID.z;

  const float _rho = getRho(ind);
  vec3 _v = getV(ind)/_rho;

  // Add Gravity
  _v += dt*0.0001*c[4]/(2.0f*_rho);


  RHO[ind] = _rho;
  V[ind] = vec4(_v, 0.0);

  // Accelerate Downwards (Gravity)!
//  for(int q = 0; q < Q; q++)
//    F[ind*Q + q] += w[q]


  // TRT Method

  const float omega_plus = 1.0/tau;
  const float lambda = 0.25;
  const float omega_minus = 1.0/(lambda/(1.0/omega_plus-0.5)+0.5);

  float ffeq[Q];
  for(int q = 0; q < Q; q++)
    ffeq[q] = equilibrium(q, _rho, _v);

  for(int q = 0; q < Q; q++){

    // Compute Equilibrium Distribution (TRT)

    float f_plus, f_minus, feq_plus, feq_minus;

    f_plus  = 0.5*(F[ind*Q + q] + F[ind*Q + cp[q]]);
    f_minus = 0.5*(F[ind*Q + q] - F[ind*Q + cp[q]]);

    feq_plus = 0.5*(ffeq[q] + ffeq[cp[q]]);
    feq_minus = 0.5*(ffeq[q] - ffeq[cp[q]]);

    FPROP[ind*Q + q] = F[ind*Q + q] - omega_plus*(f_plus - feq_plus) - omega_minus*(f_minus - feq_minus);

    if(B[ind] > 0.0)
      FPROP[ind*Q+q] = equilibrium(q, 1.0, vec3(0));

  }

}

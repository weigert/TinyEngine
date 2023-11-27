#version 460 core

layout(local_size_x = 32, local_size_y = 32) in;

#include lbm.cs

void main(){

  const uint ind = gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y;

  // Push Scheme!

  for(int q = 0; q < Q; q++){

    // Stream-To Position (Push Scheme)

    ivec2 n = ivec2(gl_GlobalInvocationID.xy) + c[q];
    if(n.x < 0 || n.x >= NX) continue;
    if(n.y < 0 || n.y >= NY) continue;

    const int nind = n.x*NY + n.y;



    /*

    for(int q = 0; q < Q; q++){

      ivec2 n = ivec2(gl_GlobalInvocationID.xy) + c[q];
      if(n.x < 0 || n.x >= NX) continue;
      if(n.y < 0 || n.y >= NY) continue;

      const int nind = (n.x*NY + n.y);
      F[nind*Q+q] = FPROP[ind*Q+q];

    }

    */





    // Wetnode

    if(B[ind] > 0.0)
      FPROP[q*NX*NY+ind] = equilibrium(q, 1.0, vec2(0));

    F[q*NX*NY + nind] = FPROP[q*NX*NY + ind];

    // Bounce-Back


//    if(B[nind] == 0.0)
//      F[q*NX*NY+nind] = FPROP[q*NX*NY+ind];
//    else{
//      F[cp[q]*NX*NY+ind] = FPROP[q*NX*NY+ind];
    //  F[nind*Q+q] = 0.0;
    //}

  }


  vec2 force = 0.15f*vec2(1, 0);

  if(
    gl_GlobalInvocationID.x == 0
  ||  gl_GlobalInvocationID.x == NX-1
  ||  gl_GlobalInvocationID.y == 0
  ||  gl_GlobalInvocationID.y == NY-1
  )
    for(int q = 0; q < Q; q++)
      F[q*NX*NY + ind] = equilibrium(q, 1.0, force);



}

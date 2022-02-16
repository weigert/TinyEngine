#version 460 core

layout(local_size_x = 25, local_size_y = 25) in;

#include lbm.cs

void main(){

  const uint ind = gl_GlobalInvocationID.x*NY + gl_GlobalInvocationID.y;

  if(B[ind] > 0.0)
    return;

  for(int q = 0; q < Q; q++){

    // Stream-To Position

    const ivec2 n = ivec2(gl_GlobalInvocationID.xy) + c[q];
    if(n.x < 0 || n.x >= NX) continue;
    if(n.y < 0 || n.y >= NY) continue;
    const int nind = n.x*NY + n.y;

    // Stream

    if(B[nind] == 0.0)
      F[nind*Q+q] = FPROP[ind*Q+q];

    // Bounce-Back

    else{
      F[ind*Q + cp[q]] = FPROP[ind*Q + q];
      F[nind*Q+q] = 0.0;
    }

  }

  // Optional: Driving Force

  if(gl_GlobalInvocationID.x == 0){

    F[ind*Q + cp[3]] = FPROP[ind*Q + 3] - 2.0*w[3]*(dot(c[3], vec2(0.2, 0)))*cs2;
    F[ind*Q + cp[7]] = FPROP[ind*Q + 7] - 2.0*w[7]*(dot(c[7], vec2(0.2, 0)))*cs2;
    F[ind*Q + cp[6]] = FPROP[ind*Q + 6] - 2.0*w[6]*(dot(c[6], vec2(0.2, 0)))*cs2;

  }

}

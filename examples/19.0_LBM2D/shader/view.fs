#version 460 core

in vec2 ex_Tex;
out vec4 fragColor;

#include lbm.cs

layout (std430, binding = 3) buffer rho {
  float RHO[];
};

layout (std430, binding = 4) buffer v {
  vec2 V[];
};

void main(){

  vec2 p = ex_Tex;
  int x = int(p.x * NX);
  int y = int(p.y * NY);
  uint ind = x*NY + y;

  if(B[ind] > 0)
    fragColor = vec4(vec3(B[ind]), 1);//vec4(vec3(v), 1);

  //else
  //  fragColor = vec4(4*abs(V[ind]), 0, 1);

  // Visualize the Vorticity



  else {

    float dvxdy = 0.5*(V[(int(p.y*NY)+1) + int(p.x*NX)*NY].x - V[(int(p.y*NY)-1) + int(p.x*NX)*NY].x);
    float dvydx = 0.5*(V[(int(p.y*NY)) + int(p.x*NX+1)*NY].y - V[(int(p.y*NY)) + int(p.x*NX-1)*NY].y);

    double w = 20.0*(dvydx-dvxdy);

    if(w > 2) w = 2;
    if(w < -2) w = -2;

    if(w > 0)
      fragColor = vec4(abs(w), 0.5*abs(w), 0, 1);

    else if(w < 0)
      fragColor = vec4(0.5*abs(w), 0, abs(w), 1);

    else fragColor = vec4(0,0,0,1);

  }
  

}

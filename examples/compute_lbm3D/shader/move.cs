// Shader to Move Particles Around!
#version 460 core

layout(local_size_x = 1024) in;

layout (std430, binding = 2) buffer b {
  float B[];
};

layout (std430, binding = 4) buffer v {
  vec4 V[];
};

layout (std430, binding = 5) buffer p {
  vec4 P[];
};

uniform int NX;
uniform int NY;
uniform int NZ;
uniform bool reset = false;

void main(){

  const uint ind = gl_GlobalInvocationID.x;

  // Get the Weights in 3 Directions

  ivec4 p = ivec4(P[ind]);
  ivec4 n = ivec4(P[ind])+ivec4(1);
  vec4 w = P[ind]-p;

  vec4 v000 = V[(p.x*NY + p.y)*NZ + p.z];
  vec4 v100 = V[(n.x*NY + p.y)*NZ + p.z];
  vec4 v010 = V[(p.x*NY + n.y)*NZ + p.z];
  vec4 v001 = V[(p.x*NY + p.y)*NZ + n.z];
  vec4 v110 = V[(n.x*NY + n.y)*NZ + p.z];
  vec4 v101 = V[(n.x*NY + p.y)*NZ + n.z];
  vec4 v011 = V[(p.x*NY + n.y)*NZ + n.z];
  vec4 v111 = V[(n.x*NY + n.y)*NZ + n.z];

  vec4 v00 = (1.0-w.x)*v000 + w.x*v100;
  vec4 v01 = (1.0-w.x)*v001 + w.x*v101;
  vec4 v10 = (1.0-w.x)*v010 + w.x*v110;
  vec4 v11 = (1.0-w.x)*v011 + w.x*v111;

  vec4 v0 = (1.0-w.y)*v00 + w.y*v10;
  vec4 v1 = (1.0-w.y)*v01 + w.y*v11;

  vec4 v = (1.0-w.z)*v0 + w.z*v1;

  P[ind] += v;

  /*

  if(P[ind].x > NX-1) P[ind].x = 0.0;
  if(P[ind].y > NY-1) P[ind].y = NY-1;
  if(P[ind].z > NZ-1) P[ind].z = 0.0;

  p = ivec4(P[ind]);
  int pind = (p.x*NY + p.y)*NZ + p.z;
  if(B[pind] > 0.0){
    P[ind].y += 1.0f;
    p = ivec4(P[ind]);
    pind = (p.x*NY + p.y)*NZ + p.z;
  }
  */

}

#version 430

layout(local_size_x = 1024) in;

layout (std430, binding = 0) buffer vertices {
  vec4 p[];
};

layout (std430, binding = 1) buffer velocity {
  vec4 v[];
};

layout (std430, binding = 2) buffer force {
  vec4 f[];
};

uniform int size;
uniform int res;
uniform int t;
uniform int mode;
uniform int rk;

const float dt = 0.2f;
const float G = 10.0f;

#include force.cs

void main() {

  const uint index = gl_GlobalInvocationID.x;
  if(index >= size) return;

  const int ind_x = int(gl_GlobalInvocationID.x / res);
  const int ind_y = int(gl_GlobalInvocationID.x % res);

  if(ind_x == res/2 && ind_y == res-1) return;
//  if(ind_x == res/3 && ind_y == res-1) return;
//  if(ind_x == 2*res/3 && ind_y == res-1) return;

//  if(ind_x == 0 && ind_y == res-1) return;
//  if(ind_x == res-1 && ind_y == res-1) return;

//  if(ind_x == 0 && ind_y == 0) return;
//  if(ind_x == res-1 && ind_y == 0) return;



  /*

  if(ind_x == 0){

    p[index].x = ind_x;
    p[index].y = (ind_y-res/2) * sin(0.0005*t);;
    p[index].z = res/2 + (ind_y-res/2) * cos(0.0005*t);;

    return;

  }

  if(ind_x == res-1){

    p[index].x = ind_x;
    p[index].y = 0;
    p[index].z = ind_y;

    return;
  }

  */

  // Compute Acceleratoin for All!

  if(mode == 0){

    f[rk*size + index] = cforce( index, rk );

  }

  // Compute

  if(mode == 1){

   leapfrog(index);
  //  verlet(index);
  //  RK4( index, rk );

  }

};

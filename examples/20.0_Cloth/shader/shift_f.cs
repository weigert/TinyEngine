#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) readonly buffer vertices {
  vec4 p[];
};

layout (std430, binding = 1) readonly buffer velocity {
  vec4 v[];
};

layout (std430, binding = 2) buffer force {
  vec4 f[];
};

#include common.cs
#include force.cs

void main() {

  const uint index = gl_GlobalInvocationID.x*res + gl_GlobalInvocationID.y;
  const uint ind_x = uint(index / res);
  const uint ind_y = uint(index % res);
  if(index >= size) 
    return;

  if(ind_y == res-1 && ind_x == res/2)
    return;

  //if(ind_y == res-1 && ind_x == 0)
  //  return;

  //if(ind_y == res-1 && ind_x == res-1)
  //  return;

  f[size + index] = f[index];
  f[index] = cforce( index, 0 );

};

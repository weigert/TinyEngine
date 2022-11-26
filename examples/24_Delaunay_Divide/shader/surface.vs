#version 460 core

layout(location = 0) in vec4 in_Position;
layout(location = 1) in ivec4 in_Index;

buffer centroids {
  vec2 c[];
};

out float ex_Depth;
out vec3 ex_Color;
out float gl_ClipDistance[3];

vec3 color(int i){
  float r = ((i >>  0) & 0xff)/255.0f;
  float g = ((i >>  8) & 0xff)/255.0f;
  float b = ((i >> 16) & 0xff)/255.0f;
  return vec3(r,g,b);
}

void main() {

  vec2 tpos = vec2(0);
  if (in_Position.x > 0) tpos = c[in_Index.x];
  if (in_Position.y > 0) tpos = c[in_Index.z];
  if (in_Position.z > 0) tpos = c[in_Index.y];

  // Compute the 3D Centroid of this Triangle
  vec3 M = vec3(c[in_Index.x], dot(c[in_Index.x], c[in_Index.x])/2)
          +vec3(c[in_Index.y], dot(c[in_Index.y], c[in_Index.y])/2)
          +vec3(c[in_Index.z], dot(c[in_Index.z], c[in_Index.z])/2);
  M /= 3.0f;

  gl_ClipDistance[0] = 1;
  gl_ClipDistance[1] = 1;
  gl_ClipDistance[2] = 1;

  if(M.z > dot(M.xy, M.xy)/2 + 0.25){

    gl_ClipDistance[0] = -1;
    gl_ClipDistance[1] = -1;
    gl_ClipDistance[2] = -1;

  }

  ex_Depth = dot(tpos, tpos)/2;
  ex_Color = color(gl_InstanceID);
  gl_Position = vec4(tpos, ex_Depth, 1);


}

#version 460 core

layout(location = 0) in vec4 in_Pos;
layout(location = 1) in ivec4 in_Index;

uniform mat4 vp;

buffer vertices {
  vec4 v[];
};

flat out vec3 normal;

void main() {

  vec4 tpos = vec4(0);
  if (in_Pos.x > 0) tpos = v[in_Index.x];
  if (in_Pos.y > 0) tpos = v[in_Index.z];
  if (in_Pos.z > 0) tpos = v[in_Index.y];
  gl_Position = vp*vec4(tpos.xyz, 1);

  normal = normalize(cross(v[in_Index.x].xyz - v[in_Index.y].xyz, v[in_Index.x].xyz - v[in_Index.z].xyz));

}

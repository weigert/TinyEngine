#version 460 core

const uvec3 LocalInvocationSize = uvec3(32, 32, 1);
layout(local_size_x = LocalInvocationSize.x, local_size_y = LocalInvocationSize.y, local_size_z = LocalInvocationSize.z) in;

layout (std430, binding = 0) readonly buffer pointsetA {
  vec3 pA[];
};

layout (std430, binding = 1) readonly buffer pointsetB {
  vec3 pB[];
};

layout (std430, binding = 2) writeonly buffer probability {
  float P[];
};

uniform uvec2 size;

void main() {

  const uint Pi = gl_GlobalInvocationID.x*size.y+gl_GlobalInvocationID.y;
  const vec3 d = pA[gl_GlobalInvocationID.x]-pB[gl_GlobalInvocationID.y];
  const float var = 1.0f;
  P[Pi] = exp(-(0.5f/var)*dot(d,d)); //Gaussian Probability)

};

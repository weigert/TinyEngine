#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) buffer position {
  vec4 p[];
};

layout (std430, binding = 1) buffer color {
  vec4 col[];
};

uniform int size;
const float dt = 0.001f;
const float G = 10.0f;

vec3 force(vec3 pB, vec3 pA){

  const vec3 dir = pB - pA;
  const float dist = length(dir);
  if(dist == 0.0f) return vec3(0);
  return normalize(dir)/(dist*dist);

}

void main() {

  const uvec3 wsize = gl_WorkGroupSize*gl_NumWorkGroups;
  const uint index = gl_GlobalInvocationID.x*wsize.y+gl_GlobalInvocationID.y;

  if(index > size) return;

  const float A = 0.9;
  const float B = 5.0;
  const float C = 9.9;
  const float D = 1.0;

  vec3 c = vec3(5, 1, 5);                   //Constant Term
  vec3 P = p[index].xyz;                    //Linear Term
  vec3 Q = vec3(P.x*P.x, P.y*P.y, P.z*P.z); //Quadratic Term
  vec3 S = vec3(P.x*P.y, P.y*P.z, P.z*P.x); //Cross-Quadratic Term

  mat3 M = mat3(-A, 0, 0,                   //Linear Matrix
                 0, 0, 0,
                 0, 0, 1);

  mat3 W = mat3(0, 1, -1,                   //Quadratic Matrix
                0, 0, 0,
                0, 0, 0);

  mat3 R = mat3(0, 0, 0,                    //Cross-Quadratic
                1, 0,-B,
                B, 0, 1);

  col[index].xyz = (c + transpose(M)*P + transpose(W)*Q + transpose(R)*S);
  col[index].w = 1.0f;
  p[index].xyz += dt*col[index].xyz;

};

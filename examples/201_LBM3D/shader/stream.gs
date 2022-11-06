#version 460 core

layout (points) in;
layout (line_strip, max_vertices = 32) out;

layout (std430, binding = 4) buffer v {
  vec4 V[];
};

uniform int NX;
uniform int NY;
uniform int NZ;

in VS_OUT {
  vec4 position;
} gs_in [];

out GS_OUT {
  vec4 color;
} gs_out;

uniform mat4 vp;
uniform mat4 model;
uniform float t = 0.0f;

void main() {

  float maxlength = 0.1;
  vec3 p = gs_in[0].position.xyz;

  for(int i = 0; i < 32; i++){

    ivec3 ip = ivec3(p);

    /*

    ivec3 n = ip+ivec3(1);
    vec3 w = p-ip;

    vec4 v000 = V[(ip.x*NY + ip.y)*NZ + ip.z];
    vec4 v100 = V[(n.x*NY + ip.y)*NZ + ip.z];
    vec4 v010 = V[(ip.x*NY + n.y)*NZ + ip.z];
    vec4 v001 = V[(ip.x*NY + ip.y)*NZ + n.z];
    vec4 v110 = V[(n.x*NY + n.y)*NZ + ip.z];
    vec4 v101 = V[(n.x*NY + ip.y)*NZ + n.z];
    vec4 v011 = V[(ip.x*NY + n.y)*NZ + n.z];
    vec4 v111 = V[(n.x*NY + n.y)*NZ + n.z];

    vec4 v00 = (1.0-w.x)*v000 + w.x*v100;
    vec4 v01 = (1.0-w.x)*v001 + w.x*v101;
    vec4 v10 = (1.0-w.x)*v010 + w.x*v110;
    vec4 v11 = (1.0-w.x)*v011 + w.x*v111;

    vec4 v0 = (1.0-w.y)*v00 + w.y*v10;
    vec4 v1 = (1.0-w.y)*v01 + w.y*v11;

    vec4 v = (1.0-w.z)*v0 + w.z*v1;

    */

  //  P[ind] += v;



    vec3 dir = V[(ip.x*NY + ip.y)*NZ + ip.z].xyz;
  //  vec3 dir = v.xyz;
    vec3 force = 0.1*vec3(cos(t), 0, sin(t));

    float factor = 20.0*length(dir-force);

    gl_Position = vp * model * vec4(p, 1.0f);
  //  gs_out.color = vec4(1.0-abs(dir)/maxlength, 1.0-0.9*length(dir)/maxlength);
      gs_out.color = vec4(abs(dir)/maxlength, factor*1.0-0.7*length(dir)/maxlength);
  //  gs_out.color = vec4(mix(vec3(0.5, 0.0, 0.5), vec3(1, 0.5, 0), length(dir)/maxlength), 1.0-0.9*length(dir)/maxlength);
    EmitVertex();

    p += dir;

    if(p.x >= NX-1) break;
    if(p.y >= NY-1) break;
    if(p.z >= NZ-1) break;
    if(p.x < 0) break;
    if(p.y < 0) break;
    if(p.z < 0) break;

  }

  EndPrimitive();


}

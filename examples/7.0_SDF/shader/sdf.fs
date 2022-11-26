#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform float zoom;
uniform vec2 offset;

uniform vec4 converge;
uniform vec4 diverge;

//Signed Distance Field Functions
float circ(vec2 p, float r){
  return length(p) - r;
}

float box(vec2 p, vec2 b){
  vec2 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

uniform bool drawlines;
uniform float thickness;
uniform float density;

bool isoline(float a, float b){
  return (abs(b-a) < 0.01);
}

float sdf(vec2 p){
  p = (p*2-vec2(1.0))*zoom-offset;
  float c = circ(p+vec2(0), 0.5);
  //return c;
  float b = box(p, vec2(0.5));
  //return b;
  //if(isoline(b, c)) return 0.0;
  return min(b, c);
}

vec4 sdfcol(float t){
  if(abs(t)<0.01) return diverge;
  vec2 p = (ex_Tex*2-vec2(1.0))*zoom-offset;
  return vec4(vec3(t)-vec3(length(p))+1,1);
}

void main(){
  fragColor = sdfcol(sdf(ex_Tex));
}

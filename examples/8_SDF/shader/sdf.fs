#version 330 core
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
  float c = circ(p+vec2(0.5), 0.5);
  float b = box(p-vec2(0.5), vec2(0.5));
  if(isoline(b, c)) return 0.0;
  return min(b, c);
}

vec4 sdfcol(float t){
  if(mod(abs(t), density) < thickness && drawlines) return vec4(vec3(0.0), 1.0);

  if(t > 0) return converge;
  else return diverge;
}

void main(){
  fragColor = sdfcol(sdf(ex_Tex));//julia(ex_Tex);
}

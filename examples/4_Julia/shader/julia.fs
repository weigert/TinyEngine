#version 130
in vec2 ex_Tex;
out vec4 fragColor;

//Uniform Parameters
uniform int maxiter;
uniform float radius;
uniform vec2 bias;
uniform float zoom;
uniform int order;
uniform vec2 offset;

//Colors
uniform vec4 converge;
uniform vec4 diverge;

//Complex Product
vec2 cprod(vec2 a, vec2 b){
  return vec2(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

//Complex Power
vec2 cpow(vec2 a, int n){
  vec2 b = a;
  if(n == 0) return vec2(1, 0);
  for(int i = 1; i < n; i++)
    b = cprod(b, a);
  return b;
}

vec4 julia(vec2 p){
  //Move the Image
  p = (p*2-vec2(1.0))*zoom-offset;

  //Find the Convergence Behavior
  int iter = 0;
  while(length(p) < radius && iter < maxiter){
      p = 2*cpow(p, order)+4*cpow(p, order+1)-3*cpow(p, order+2)+bias;
      iter++;
  }

  //Pick the Pixel Color
  if(iter == maxiter) return converge;
  else return mix(diverge, converge, float(iter)/float(maxiter));
}

void main(){
  fragColor = julia(ex_Tex);
}

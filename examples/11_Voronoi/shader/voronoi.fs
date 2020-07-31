#version 330 core

in vec2 ex_Tex;
in vec2 ex_Centroid;
flat in int ex_ID;

out vec4 fragColor;

uniform int NCOLOR;
vec3 black = vec3(0);

vec3 color(int i, int N){
  int Z = (i%N);
  int Y = ((i/N)%N);
  int X = ((i/(N*N))%N);
  return vec3(X, Y, Z)/vec3(N-1.0f);
}

void main(){
  vec2 p = ex_Tex;
  gl_FragDepth = length(p-ex_Centroid);
  fragColor = vec4(color(ex_ID, NCOLOR), 1.0);
}

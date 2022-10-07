#version 330

in vec3 ex_Quad;
in vec3 ex_Centroid;

flat in vec3 ex_Color;

out vec4 fragColor;

uniform float R;

float L1(vec3 a, vec3 b){
  return (abs(a.x-b.x)+abs(a.y-b.y));
}

float L2(vec3 a, vec3 b){
  return length(a-b);
}

float GEODESIC(vec3 a, vec3 b){
  return atan(length(cross(a,b))/dot(a,b));
}

void main(){

  //  gl_FragDepth = L1(ex_Quad, ex_Centroid);
  //  gl_FragDepth = L1(ex_Quad, ex_Centroid);
  gl_FragDepth = GEODESIC(ex_Quad, ex_Centroid)/R;

  fragColor = vec4(ex_Color, 1);
//  fragColor = vec4(1,0,0,1);

}

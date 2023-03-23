#version 330 core

in vec2 ex_Quad;
in vec2 ex_Centroid;
flat in vec3 ex_Color;
out vec4 fragColor;

uniform float R;

uniform bool drawcenter;
uniform int metric;
uniform float twist;
uniform int style;

float L1(vec2 a, vec2 b){
  return (abs(a.x-b.x)+abs(a.y-b.y));
}

float L2(vec2 a, vec2 b){
  return length(a-b);
}

float Ltwist(vec2 a, vec2 b){
  vec2 dir = b-a;
  float d = length(dir);


  float PI = 3.14159265f;

//  dir.x = sin(theta);
//  dir.y = cos(theta);

  //dir.

  float theta = atan(dir.y/dir.x);
  if(dir.x < 0) theta += PI;
  //if(dir.x < 0) theta = theta-2*PI;
//  if(dir.y < 0)
//    theta = 2*PI-theta;

  //atan(dir.y/dir.x);
  //if(dir.x < 0) theta = -theta;

  /*
    I want a sine wave around the radius with a certain period.
    Meaning that as I go around, I have a signal strength going up and down.

    I want to compute the angle of the vector.

    1. Compute Angle of Vector
    2. Multiply with a sine wave squared of a certain frequency to get tentacle effect.
    3. But add a phase shift that is proportional to the distance from the center.
    4. That is the distance function!

  */

  float k = twist;
  float m = 5;
  return d+0.05*sin(k*theta+m*d/R)*sin(k*theta+m*d/R);//(abs(a.x-b.x)+abs(a.y-b.y));
}

void main(){

  if(metric == 0)
    gl_FragDepth = L1(ex_Quad, ex_Centroid);
  else if(metric == 1){
    gl_FragDepth = L2(ex_Quad, ex_Centroid);
    if(gl_FragDepth > R) discard; //Make them Round
  }
  else{
    gl_FragDepth = Ltwist(ex_Quad, ex_Centroid);
  }

  if(style == 1)
    fragColor = vec4(vec3(gl_FragDepth/R), 1.0);
  else
    fragColor = vec4(ex_Color, 1.0);

  if(gl_FragDepth < 0.1*R && drawcenter)
    fragColor = vec4(1.0);
}

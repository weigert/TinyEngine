#version 460 core

layout(local_size_x = 1024) in;

layout (std430, binding = 0) buffer position {
  vec2 p[];
};

layout (std430, binding = 1) buffer speed {
  vec2 sp[];
};

uniform sampler2D imageTexture;

uniform int N;

void main(){

  const uint index = gl_GlobalInvocationID.x;

  if(index >= N)
    return;

  vec2 pp = p[index];
  vec2 psp = sp[index];


  // Alter the Position

  pp = 0.5*(pp+1.0f);
  //pp = 0.5*pp+0.5f;
//  pp = 2.0*(pp-0.5);



  vec2 grad;
  float f = 0.05;
  grad.x = length(texture(imageTexture, pp+vec2(f, 0))) - length(texture(imageTexture, pp-vec2(f, 0)));
  grad.y = length(texture(imageTexture, pp+vec2(0, f))) - length(texture(imageTexture, pp-vec2(0, f)));

  sp[index] = psp+0.05*grad;



  /*
  float a = length(texture(imageTexture, pp+psp));
  float b = length(texture(imageTexture, pp+psp+0.1*vec2(psp.y, -psp.x)));
  float c = length(texture(imageTexture, pp+psp-0.1*vec2(psp.y, -psp.x)));

  if(c > b && c > a)
    sp[index] = mix(psp,-psp-0.1*vec2(psp.y, -psp.x)-pp, 0.05);
  if(b > a && b > c)
    sp[index] = mix(psp,-psp+0.1*vec2(psp.y, -psp.x)-pp, 0.05);
    */

//  sp[index] = normalize(sp[index];


  /*


  float agrad = length(texture(imageTexture, pp+psp + 0.2*vec2(psp.y, -psp.x)));
  float bgrad = length(texture(imageTexture, pp+psp + 0.2*vec2(-psp.y, psp.x)));

  sp[index] += (agrad - bgrad)*0.2*vec2(psp.y, -psp.x);

  */

  if(p[index].x >= 1)
    sp[index].x *= -1;

  if(p[index].x <= -1)
    sp[index].x *= -1;

  if(p[index].y >= 1)
    sp[index].y *= -1;

  if(p[index].y <= -1)
    sp[index].y *= -1;

  p[index] += 0.001*sp[index];

}

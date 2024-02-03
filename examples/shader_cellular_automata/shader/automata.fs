#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

float live = 0.0f;//vec3(0.0, 0.0, 0.0);
float dead = 1.0f;//vec3(1.0, 1.0, 1.0);

//Conways Game of Life
float rule(){

  vec2 p = ex_Tex*(1.0, 1.0); //Scale
  p.y = 1.0f-p.y;
  bool s = (texture(imageTexture, p).r == live);

  int n = 0; //Neighbors
  n += (textureOffset(imageTexture, p, ivec2( 1, 0)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2(-1, 0)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 0, 1)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 0,-1)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 1, 1)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2(-1,-1)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2(-1, 1)).r == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 1,-1)).r == live)?1:0;

  if(s && (n == 2 || n == 3)) return live;
  if(!s && n == 3) return live;
  return dead;
}

void main(){
  fragColor = vec4(rule(), 0, 0, 1.0);
}

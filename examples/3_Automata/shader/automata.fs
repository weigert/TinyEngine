#version 330 core
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

vec3 live = vec3(0.0, 0.0, 0.0);
vec3 dead = vec3(1.0, 1.0, 1.0);

//Conways Game of Life
vec3 rule(){
  vec2 p = ex_Tex*(1.0, 1.0); //Scale
  bool s = (texture(imageTexture, p).xyz == live); //Self

  int n = 0; //Neighbors
  n += (textureOffset(imageTexture, p, ivec2( 1, 0)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2(-1, 0)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 0, 1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 0,-1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 1, 1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2(-1,-1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2(-1, 1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, p, ivec2( 1,-1)).xyz == live)?1:0;

  if(s && (n == 2 || n == 3)) return live;
  if(!s && n == 3) return live;
  return dead;
}

void main(){
  fragColor = vec4(rule(), 1.0);
}

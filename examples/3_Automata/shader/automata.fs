#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

float size = 100.0f;

vec3 live = vec3(0.0, 0.0, 0.0);
vec3 dead = vec3(1.0, 1.0, 1.0);

//Conways Game of Life
vec3 rule(){
  //Self
  bool s = (texture(imageTexture, ex_Tex).xyz == live);

  //Neighbors
  int n = 0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 1, 0)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2(-1, 0)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 0, 1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 0,-1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 1, 1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2(-1,-1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2(-1, 1)).xyz == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 1,-1)).xyz == live)?1:0;

  //Rule
  if(s && (n == 2 || n == 3)) return live;
  if(!s && n == 3) return live;
  return dead;
}

void main(){
  fragColor = vec4(rule(), 1.0);
}

#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

float size = 100.0f;

vec4 live = vec4(0.0, 0.0, 0.0, 1.0);
vec4 dead = vec4(1.0, 1.0, 1.0, 1.0);

//Conways Game of Life
vec4 rule(){
  //Self
  bool s = (texture(imageTexture, ex_Tex) == live);

  //Neighbors
  int n = 0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 1, 0)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2(-1, 0)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 0, 1)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 0,-1)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 1, 1)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2(-1,-1)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2(-1, 1)) == live)?1:0;
  n += (textureOffset(imageTexture, ex_Tex, ivec2( 1,-1)) == live)?1:0;

  //Rule
  if(s && (n == 2 || n == 3)) return live;
  if(!s && n == 3) return live;
  return dead;
}

void main(){
  fragColor = rule();
}

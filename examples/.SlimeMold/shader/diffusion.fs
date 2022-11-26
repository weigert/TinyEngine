#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

vec3 black = vec3(0.0, 0.0, 0.0);
vec3 white = vec3(1.0, 1.0, 1.0);

float D = 0.1;

//Conways Game of Life
vec3 rule(){
  vec2 p = ex_Tex*(1.0, 1.0); //Scale
  vec3 c = texture(imageTexture, p).xyz; //Self

  vec3 dx = -1.0/12.0*textureOffset(imageTexture, p, ivec2( 2, 0)).xyz;
       dx += 4.0/3.0*textureOffset(imageTexture, p, ivec2( 1, 0)).xyz;
       dx += -5.0/2.0*textureOffset(imageTexture, p, ivec2( 0, 0)).xyz;
       dx += 4.0/3.0*textureOffset(imageTexture, p, ivec2(-1, 0)).xyz;
       dx += -1.0/12.0*textureOffset(imageTexture, p, ivec2(-2, 0)).xyz;

  vec3 dy = -1.0/12.0*textureOffset(imageTexture, p, ivec2( 0, 2)).xyz;
       dy += 4.0/3.0*textureOffset(imageTexture, p, ivec2( 0, 1)).xyz;
       dy += -5.0/2.0*textureOffset(imageTexture, p, ivec2( 0, 0)).xyz;
       dy += 4.0/3.0*textureOffset(imageTexture, p, ivec2( 0,-1)).xyz;
       dy += -1.0/12.0*textureOffset(imageTexture, p, ivec2( 0,-2)).xyz;

/*
  vec3 dx = 1*textureOffset(imageTexture, p, ivec2( 1, 0)).xyz;
  dx += 1*textureOffset(imageTexture, p, ivec2(-1, 0)).xyz;
  dx -= 2*textureOffset(imageTexture, p, ivec2( 0, 0)).xyz;

  vec3 dy = 1*textureOffset(imageTexture, p, ivec2( 0, 1)).xyz;
  dy += 1*textureOffset(imageTexture, p, ivec2( 0,-1)).xyz;
  dy -= 2*textureOffset(imageTexture, p, ivec2( 0, 0)).xyz;
*/

  return c+D*(dx+dy);
}

void main(){
  fragColor = vec4(rule(), 1.0);
}

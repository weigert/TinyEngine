#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

//Effect Parameters
uniform int index;
uniform int res;
uniform int bits;

vec4 none(){
  return texture(imageTexture, ex_Tex);
}

vec4 pixelate(){
  return texture(imageTexture, floor(ex_Tex*vec2(res))/vec2(res));
}

vec4 bitreduce(){
  vec4 color = texture(imageTexture, ex_Tex);
  return vec4(round(color.xyz*vec3(bits))/vec3(bits), 1.0);
}

void main(){
  //Get the fragment color from the effect choice
  if(index == 1) fragColor = pixelate();
  else if(index == 2) fragColor = bitreduce();
  else fragColor = none();
}

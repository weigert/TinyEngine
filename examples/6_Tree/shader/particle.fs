#version 130

in vec2 ex_Tex;

uniform sampler2D spriteTexture;
uniform sampler2D normalTexture;
uniform vec4 leafcolor;

uniform vec3 lightDir;
uniform vec3 lookDir;
uniform vec3 lightCol;
uniform float lightStrength;

out vec4 fragColor;

void main(void) {

  vec4 color = texture(spriteTexture, ex_Tex);
  if(color.a == 0.0) discard;
  fragColor = leafcolor;

}

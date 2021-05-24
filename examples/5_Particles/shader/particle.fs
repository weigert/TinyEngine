#version 130

in vec2 ex_Tex;

uniform sampler2D spriteTexture;

out vec4 fragColor;

void main(void) {
  vec4 color = texture(spriteTexture, ex_Tex);
  if(color.a == 0.0) discard;
  else fragColor = color;
}

#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

//Effect Parameters
uniform bool horizontal;

void main(){

  float pi = 6.28318530718; // Pi*2

  float directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
  float quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
  float size = 8.0; // BLUR SIZE (Radius)

  float rad = size/1200.0f;
  vec4 color = texture(imageTexture, 1.0-ex_Tex);

  // Blur calculations
  for( float d=0.0; d<pi; d+=pi/directions){
    for(float i=1.0/quality; i<=1.0; i+=1.0/quality){
      color += texture(imageTexture, 1.0-ex_Tex-vec2(cos(d),sin(d))*rad*i);
    }
  }

  // Output to screen
  color /= quality * directions - 15.0;
  fragColor =  color;

}

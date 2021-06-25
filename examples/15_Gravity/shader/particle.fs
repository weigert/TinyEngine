#version 130

in vec2 ex_Tex;
out vec4 fragColor;

const vec3 color = vec3(1,1,1);

void main(void) {

  float d = length((ex_Tex-0.5f)*2.0f);         //Radial Distance
  fragColor = vec4(color, (1.0f-d)*(1.0f-d));   //Quadratic Opacity Fall-Off

}

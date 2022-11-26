#version 460 core
out vec4 fragColor;

in GS_OUT {
  vec4 color;
} gs_out;

void main(){
  fragColor = vec4(gs_out.color);
}

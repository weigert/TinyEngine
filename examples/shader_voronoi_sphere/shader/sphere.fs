#version 330 core

in vec3 ex_Normal;

uniform samplerCube voronoidepth;
uniform samplerCube voronoicolor;

out vec4 fragColor;

void main() {

  // Visualize the Depth Texture
//   float d =  texture(voronoidepth, ex_Normal).r;
//   fragColor = vec4(vec3(d), 1);//vec4(ex_Color, 1);

  // Visualize the Color Texture
   fragColor = texture(voronoicolor, ex_Normal);

}

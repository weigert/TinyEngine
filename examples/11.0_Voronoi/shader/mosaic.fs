#version 430 core
in vec2 ex_Tex;
out vec4 fragColor;

layout (std430, binding = 0) buffer centroids {
  vec2 c[];
};

uniform sampler2D voronoi;
uniform sampler2D image;

uniform int NCOLOR;

int index(vec3 a){
  a = a*255.0f;
  return int((a.x + a.y*256 + a.z*256*256));
}

void main(){
  int i = index(texture(voronoi, ex_Tex).rgb);
  fragColor = texture(image, 0.5*(c[i]+1.0));
}

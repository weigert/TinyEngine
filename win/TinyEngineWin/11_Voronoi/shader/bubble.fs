#version 430 core
in vec2 ex_Tex;
out vec4 fragColor;

layout (std430, binding = 0) buffer centroids {
  vec2 c[];
};

uniform sampler2D voronoi;
uniform sampler2D image;

uniform float R;

int index(vec3 a){
  a = a*255.0f;
  return int((a.x + a.y*256 + a.z*256*256));
}

void main(){
  int i = index(texture(voronoi, ex_Tex).rgb);

  //Direction
  vec2 dir = (2*ex_Tex-1) - c[i]; //Direction to the Centroid
  float d = length(dir)/R; //Distance divided by R
  fragColor = mix(texture(image, ex_Tex+dir*d), vec4(vec3(0), 1), d*d);
}

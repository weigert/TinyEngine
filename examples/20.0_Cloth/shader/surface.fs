#version 430 core

in vec3 normal;
out vec4 fragColor;

void main(){

  float d = clamp(abs(dot(normal, normalize(vec3(0,0.5,-1)))), 0.2, 1);

  vec3 color = d*vec3(1.0,0.5,0.5);

  fragColor = vec4(color,1);

}

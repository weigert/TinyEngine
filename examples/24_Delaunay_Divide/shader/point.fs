#version 460 core

out vec4 fragColor;

const vec3 color = vec3(1,0,0);

void main() {
  fragColor = vec4(color, 1.0f);
}

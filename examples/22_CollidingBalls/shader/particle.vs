#version 460 core

layout(location = 0) in vec2 in_Quad;
layout(location = 1) in vec2 in_Tex;
layout(location = 2) in vec4 in_Pos;

uniform float R;
out vec2 pos;
flat out int index;

const float RATIO = 8.0/4.0;

void main() {
  index = gl_InstanceID;
  pos = R*in_Quad;
  gl_Position = vec4((pos*vec2(1, RATIO)+in_Pos.xy), 1, 1);
}

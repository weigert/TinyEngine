#version 330 core
layout (location = 0) in vec3 in_Quad;
layout (location = 1) in vec2 in_Tex;
layout (location = 2) in mat4 in_Model;

flat out int ex_ID;
out vec2 ex_Tex;
out vec2 ex_Centroid;

void main(){
  //Position is Translated!
  ex_Tex =  (in_Model*vec4(in_Quad, 1.0)).xy;
  ex_Centroid = (in_Model*vec4(0, 0, 0, 1.0)).xy;

  ex_ID = gl_InstanceID;
  gl_Position = in_Model*vec4(in_Quad, 1.0);
}

#version 330

layout (location = 0) in vec3 in_Quad;
layout (location = 2) in vec3 in_Centroid;

out vec3 g_Quad;
out vec3 g_Centroid;
flat out vec3 g_Color;

uniform float R = 0.2;

vec3 color(int i){
  float r = ((i >>  0) & 0xff)/255.0f;
  float g = ((i >>  8) & 0xff)/255.0f;
  float b = ((i >> 16) & 0xff)/255.0f;
  return vec3(r,g,b);
}

void main(){

  // Normalize the Centroid on the Sphere

  g_Centroid = normalize(in_Centroid);

  // Compute the Sideways, Upways Direction for the Flat

  // Reference Direction

  vec3 d = vec3(0,1,0);
  if(abs(in_Centroid.y) > 0.8)
    d = vec3(0,0,1);

  vec3 px = normalize(cross(in_Centroid, d));
  vec3 py = normalize(cross(in_Centroid, px));

  // Orient the Quad Correctly!

  g_Quad = R*(in_Quad.x*px + in_Quad.y*py) + in_Centroid;
  g_Color = color(gl_InstanceID);

  gl_Position = vec4(g_Quad, 1.0);

}

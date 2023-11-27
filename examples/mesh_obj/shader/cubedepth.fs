#version 330
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far;

void main(){
    gl_FragDepth = length(FragPos.xyz - lightPos)/far;
}

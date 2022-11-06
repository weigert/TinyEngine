#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

void main(){
    fragColor = mix(texture(imageTexture, ex_Tex), vec4(0), 0.01);
}

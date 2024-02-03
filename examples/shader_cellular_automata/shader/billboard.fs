#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

void main(){
    float val = texture(imageTexture, ex_Tex).r;
    fragColor = vec4(val, val, val, 1);
}

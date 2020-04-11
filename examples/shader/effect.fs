#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;
uniform sampler2D depthTexture;

void main(){
    //Get Fragment Color from Texture
    fragColor = texture(imageTexture, ex_Tex);

    //Add Distance Fog
    float depthVal = clamp(texture(depthTexture, ex_Tex).r, 0.0, 1.0);
    if(depthVal < 1.0)  //If it is a visible thing...
      fragColor = mix(fragColor, vec4(1.0), pow(3*(depthVal-0.55),2));  //White Fog Color!
}

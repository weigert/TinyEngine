#version 130
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

//Effect Parameters
uniform int index;
uniform int res;
uniform int bits;

vec4 none(){
  return texture(imageTexture, ex_Tex);
}

vec4 pixelate(){
  return texture(imageTexture, floor(ex_Tex*vec2(res))/vec2(res));
}

vec4 bitreduce(){
  vec4 color = texture(imageTexture, ex_Tex);
  return vec4(round(color.xyz*vec3(bits))/vec3(bits), 1.0);
}

float WIDTH = 1000;
float HEIGHT = 1000;
vec2 SIZE = vec2(WIDTH, HEIGHT);

float c(float x, float y, float rad, vec2 p){
    x = x - WIDTH/2.0;
    y = y - HEIGHT/2.0;

    float ret = (p.x+x)*(p.x+x) + (p.y+y)*(p.y+y);
    ret = sqrt(ret);
    if(ret >= rad)
        return 1.0;
    else
        return 0.0;
}
float r(float x, float y, float w, float h, vec2 p){
    x = x + WIDTH/2.0;
    y = y + HEIGHT/2.0;
    if(p.x >= x)
        if(p.x <= x+w)
            if(p.y >= y)
                if(p.y <= y + h)
        			return 0.0;
    return 1.0;
}


void main(){
  //Get the fragment color from the effect choice
  /*
  if(index == 1) fragColor = pixelate();
  else if(index == 2) fragColor = bitreduce();
  else fragColor = none();
  */
  //fragColor = vec4(vec3(1.0), 1.0);

  fragColor = vec4(c(100.0, 50.0, 50.0, ex_Tex*SIZE)*c(0.0, 50.0, 50.0, ex_Tex*SIZE)*r(-85.0, -50.0, 70.0, 150.0, ex_Tex*SIZE) * c(50.0, -100.0, 35.0, ex_Tex*SIZE), 0.0, 0.0,1.0);

}

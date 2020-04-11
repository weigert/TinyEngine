#version 130
//Lighting Settings
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lookDir;
uniform float lightStrength;

//Sampler for the ShadowMap
uniform sampler2D shadowMap;

//IO
in vec4 ex_Color;
in vec3 ex_Normal;
in vec2 ex_Position;
in vec4 ex_Shadow;
in vec3 ex_FragPos;
out vec4 fragColor;

//Sample a grid..
float gridSample(int size){
  //Stuff
  float shadow = 0.0;
  float currentDepth = ex_Shadow.z;

  //Compute Bias
  float m = 1-dot(ex_Normal, normalize(lightPos));
  float bias = mix(0.002, 0.2*m, pow(m, 5));

  for(int x = -size; x <= size; ++x){
      for(int y = -size; y <= size; ++y){
          float pcfDepth = texture(shadowMap, ex_Shadow.xy + vec2(x, y) / textureSize(shadowMap, 0)).r;
          shadow += currentDepth - 0.001 > pcfDepth ? 1.0 : 0.0;
      }
  }
  //Normalize
  shadow/=((2*size+1)*(2*size+1)*2.0);
  return shadow;
}

vec4 shade(){
    //Shadow Value
    float shadow = 0.0;
    if(greaterThanEqual(ex_Shadow.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(ex_Shadow.xy, vec2(1.0f)) == bvec2(true))
      shadow = gridSample(1);

    //Sample the Shadow Value from Texture
    return vec4(vec3(1-shadow), 1.0f);
}

vec4 phong(){
  float diffuse = 0.5*max(dot(normalize(ex_Normal), normalize(lightPos)), 0.0);
  float ambient = 0.001;
  float spec = pow(max(dot(normalize(lookDir), normalize(reflect(lightPos, ex_Normal))), 0.0), 32.0);
  spec = 0.5 * spec;

  return vec4(lightCol*lightStrength*(diffuse + ambient + spec), 1.0f);
}

void main(void) {
  fragColor = shade()*ex_Color;
}

#version 430 core

in vec2 ex_Tex;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gDepth;

uniform sampler2D ssaoTex;

out vec4 fragColor;

uniform mat4 view;

vec3 ex_Position;
vec3 ex_Normal;
vec3 ex_Color;

vec4 ex_WorldPos;
vec4 ex_Shadow;

/*

float gridSample(const int size){

  const float area = ((1 + 2*size)*(1 + 2*size));

  //Stuff
  float shadow = 0.0;
  float currentDepth = ex_Shadow.z;

  //Compute Bias
  float m = 1-dot(ex_Normal, normalize(lightPos));
  float bias = 0.0f;//mix(0.002, 0.2*m, pow(m, 5));

  for(int x = -size; x <= size; ++x){
      for(int y = -size; y <= size; ++y){
          float pcfDepth = texture(shadowMap, ex_Shadow.xy + vec2(x, y) / textureSize(shadowMap, 0)).r;
          if(pcfDepth == 1)
            return 0;

          shadow += currentDepth - 0.001 > pcfDepth ? 1.0 : 0.0;
      }
  }
  //Normalize
  shadow /= area;
  return shadow;
}

float shade(){

  float shadow = 0.0;
	const int size = 1;

  if(greaterThanEqual(ex_Shadow.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(ex_Shadow.xy, vec2(1.0f)) == bvec2(true))
    shadow = gridSample(size);


	return shadow;

}
*/

/*
vec3 blinnphong(){

  // Ambient (Factor)
  float ambient = 0.6;

  // Diffuse (Factor)

  vec3 lightDir = normalize(transpose(inverse(mat3(view)))*lightPos);
  float diffuse  = 1.0*clamp(dot(ex_Normal, lightDir), 0.1, 0.9);

  // Specular Lighting (Factor)

  float discharge = texture(dischargeMap, ex_WorldPos.xz/512).a;
  float specularStrength = 0.05 + 0.55*discharge;
//  if(ex_WorldPos.y <= 25)
//    specularStrength = 0.6;

  vec3 halfwayDir = normalize(lightDir + vec3(0,0,1));
  float spec = pow(max(dot(ex_Normal, halfwayDir), 0.0), 64);
  float specular = specularStrength * spec;

  // Multiply by Lightcolor

  float ao = texture(ssaoTex, ex_Tex).r;
  float shadow = 1.0f-shade();

  diffuse *= shadow;
  specular *= shadow;
  ambient *= ao;

  return lightStrength*(ambient + (diffuse + specular))*lightCol;

}
*/

void main() {

  ex_Position = texture(gPosition, ex_Tex).xyz;
  ex_Normal = texture(gNormal, ex_Tex).xyz;
  ex_Color = texture(gColor, ex_Tex).xyz;

  float ao = texture(ssaoTex, ex_Tex).r;

/*
  ex_WorldPos = inverse(view) *  vec4(ex_Position, 1.0f);
  ex_Shadow =  dbvp * ex_WorldPos;
*/

/*
  // Extract Base Values


  // Transform to Viewspace

//  fragColor = texture(shadowMap, ex_Tex);
  fragColor = texture(gColor, ex_Tex);
  fragColor = vec4(blinnphong()*fragColor.xyz, 1.0);

  // Depth-Fog


  float depthVal = clamp(texture(gDepth, ex_Tex).r, 0.0, 1.0);
  if(depthVal == 1) fragColor = vec4(skyCol, 1);

  //depthVal = exp(-0.4*depthVal);
  //fragColor = mix(fragColor, vec4(skyCol, 1.0), 1.0-depthVal);




*/
  fragColor = vec4(vec3(ao), 1);
  //fragColor = texture(gPosition, ex_Tex);
}

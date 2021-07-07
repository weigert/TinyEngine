#version 130

in vec2 ex_Tex;

uniform sampler2D spriteTexture;
uniform vec4 leafcolor;


in vec4 ex_Shadow;


uniform vec3 light;
uniform vec3 lightcolor;

uniform sampler2D shadowMap;

float gridSample(int size){
  float shadow = 0.0;
  float cur = ex_Shadow.z;

  vec3 normal = -light;

  float m = 1 - dot(normal, normalize(light));
  float bias = 0.001;//mix(0.005, 0.001*m, pow(m, 5));

	for(int x = -size; x <= size; ++x){
      for(int y = -size; y <= size; ++y){
          float near = texture(shadowMap, ex_Shadow.xy + vec2(x, y) / textureSize(shadowMap, 0)).r;
					shadow += cur-bias > near ? 1.0 : 0.0;
      }
  }

  float norm = (2*size+1)*(2*size+1);
  return shadow/norm;
}

float shade(){
    float shadow = 0.0;
		int size = 0;

    if(greaterThanEqual(ex_Shadow.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(ex_Shadow.xy, vec2(1.0f)) == bvec2(true))
      shadow = gridSample(size);

		return shadow;
}

vec4 directionalLight(){
	float orthoshade = 0.5*shade();
	return vec4((1.0-orthoshade)*lightcolor, 1.0);
}

uniform bool selfshadow;

out vec4 fragColor;

void main(void) {

  vec4 color = texture(spriteTexture, ex_Tex);
  if(color.a == 0.0) discard;

  vec4 _light = vec4(1.0);
  if(selfshadow) _light = directionalLight();
  fragColor = _light*leafcolor;

}

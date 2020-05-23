#version 130

in vec4 ex_Color;
in vec3 ex_Normal;
in vec3 ex_FragPos;
in vec4 ex_Shadow;

out vec4 fragColor;

uniform vec4 drawcolor;

uniform bool wireframe;
uniform bool drawfloor;

uniform vec3 light;

uniform sampler2D shadowMap;

float gridSample(int size){
  float shadow = 0.0;
  float cur = ex_Shadow.z;

  float m = 1 - dot(ex_Normal, normalize(light));
  float bias = mix(0.005, 0.001*m, pow(m, 5));

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
		int size = 1;

    if(greaterThanEqual(ex_Shadow.xy, vec2(0.0f)) == bvec2(true) && lessThanEqual(ex_Shadow.xy, vec2(1.0f)) == bvec2(true))
      shadow = gridSample(size);

		return shadow;
}

uniform vec3 lightcolor;

vec4 directionalLight(){
	float orthoshade = 0.5*shade();
	return vec4((1.0-orthoshade)*lightcolor, 1.0);
}

uniform bool drawshadow;

void main(void) {
	vec4 _light = vec4(1.0);
	if(drawshadow) _light = directionalLight();
	if(drawfloor) fragColor = _light*drawcolor;
  else if(!wireframe) fragColor = _light*drawcolor;
  else fragColor = drawcolor;
}

#version 330 core

in vec4 ex_Color;
in vec3 ex_Normal;

in vec3 ex_Model;   //Model Space
in vec4 ex_Frag;

out vec4 fragColor;

uniform vec3 camera;

//Pointlight
uniform vec3 pointlightpos;
uniform float pointlightfar;
uniform samplerCube pointshadowMap;
uniform vec3 attenuation;
uniform bool pointlighton;
uniform float brightness;
uniform vec3 pointlightcolor;

float pointShadow(samplerCube cube, vec3 pos, float _far){
	float shadow = 0.0;

  vec3 dir = ex_Model - pos;
  float m = 1.0-dot(ex_Normal, -normalize(dir));
  float bias = max(0.001, 0.01*m);

  const float samples = 4.0;
  const float offset  = 0.5;

  for(float x = -offset; x < offset; x += offset / (samples * 0.5)){
      for(float y = -offset; y < offset; y += offset / (samples * 0.5)){
          for(float z = -offset; z < offset; z += offset / (samples * 0.5)){

              float near = texture(cube, dir+vec3(x,y,z)).r;
              float cur = length(dir)/_far;
              if(cur > 1.0) shadow += 1.0;
              else shadow += (cur - bias > near) ? 1.0:0.0;

          }
      }
  }
	return shadow / (samples * samples * samples);
}

vec4 pointLight(){
	vec3 dir = pointlightpos-ex_Model;
	float dist = length(dir)/pointlightfar;
	float A = 1.0/(attenuation.x + attenuation.y*dist + attenuation.z*dist*dist);

	float ambient  = 0.3;
	float diffuse  = 0.8*max(dot(ex_Normal, normalize(dir)), 0.0);
	float specular = 0.2*pow(max(dot(normalize(ex_Model-camera), normalize(reflect(dir, ex_Normal))), 0.0), 16);

  float cubeshade = pointShadow(pointshadowMap, pointlightpos, pointlightfar);

	return brightness*vec4(A*(ambient+(1.0-cubeshade)*(diffuse+specular))*pointlightcolor, 1.0);
}

void main(void) {
  vec4 light = vec4(0);          //Compute Lighting
  if(pointlighton)
    light += pointLight();
	fragColor = light*ex_Color;    //Set Color
}

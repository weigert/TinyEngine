#version 130

in vec4 ex_Color;
in vec3 ex_Normal;
in vec3 ex_FragPos;

out vec4 fragColor;

//Light Stuff
uniform vec3 lightDir;
uniform vec3 lookDir;
uniform vec3 lightCol;
uniform float lightStrength;

uniform vec4 treecolor;
uniform vec3 wirecolor;

uniform bool wireframe;

vec4 phong(){
	//Color Calculations - Per Vertex! Not Fragment.
	float diffuse = clamp(dot(ex_Normal, normalize(lightDir)), 0.1, 0.9);
	float ambient = 0.1;
	float spec = 0.8*pow(max(dot(normalize(lookDir), normalize(reflect(lightDir, ex_Normal))), 0.0), 32.0);

	return vec4(lightCol*lightStrength*(diffuse + ambient + spec), 1.0f);
}

void main(void) {
  if(!wireframe) fragColor = treecolor;
  else fragColor = vec4(wirecolor, 1.0);
}

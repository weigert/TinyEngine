#version 130
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec3 in_Position;
in vec3 in_Normal;

//Lighting
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lookDir;
uniform float lightStrength;

//Color Stuff
uniform vec3 flatColor;
uniform vec3 steepColor;
uniform float steepness;

//Uniforms
uniform mat4 model;
uniform mat4 projectionCamera;
uniform mat4 dbmvp;

// We output the ex_Color variable to the next shader in the chain
out vec4 ex_Color;
out vec3 ex_Normal;
out vec2 ex_Position;
out vec4 ex_Shadow;
out vec3 ex_FragPos;

vec4 gouraud(){
	//Color Calculations - Per Vertex! Not Fragment.
	float diffuse = clamp(dot(normalize(in_Normal), normalize(lightPos)), 0.1, 0.9);
	float ambient = 0.1;
	float spec = 0.8*pow(max(dot(normalize(lookDir), normalize(reflect(lightPos, in_Normal))), 0.0), 32.0);

	return vec4(lightCol*lightStrength*(diffuse + ambient + spec), 1.0f);
}

void main(void) {
	vec3 inPos = in_Position;
	//if(inPos.y <= 16) inPos.y = 16;
	//Position Calculations
	ex_FragPos = (model * vec4(inPos, 1.0f)).xyz;
	ex_Shadow = dbmvp * vec4(ex_FragPos, 1.0f);
	gl_Position = projectionCamera * vec4(ex_FragPos, 1.0f);
	ex_Position = ((gl_Position.xyz / gl_Position.w).xy * 0.5 + 0.5 );
	ex_Normal = in_Normal;

	//Color Without Shading!
	//if(inPos.y <= 16) ex_Color = vec4(0.17, 0.6, 0.92, 1.0);
	if(normalize(ex_Normal).y < steepness) ex_Color = vec4(steepColor, 1.0)*gouraud();
	else ex_Color = vec4(flatColor, 1.0)*gouraud();
}

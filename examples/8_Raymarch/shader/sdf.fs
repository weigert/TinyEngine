#version 330 core
in vec2 ex_Tex;
out vec4 fragColor;

uniform float zoom;
uniform vec2 offset;

uniform vec4 converge;
uniform vec4 diverge;

uniform vec3 camerapos;
uniform vec3 lookpos;

vec3 lightpos = vec3(1, 2, 3);
vec3 lightcol = vec3(1);

//Signed Distance Field Functions
float circ(vec3 p, float r){
  return length(p) - r;
}

float box(vec3 p, vec3 b){
  vec3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

float sdf(vec3 p){
  float a = box(p+0.5, vec3(1.0));
  float b = circ(p-0.5, 1.0);
  //return max(a, -b);
  return min(a, b);
}

vec3 ray(vec3 start, vec3 dir, float mind, float maxd){
  vec3 p = start;
  float d = sdf(p);
  while(length(p - start) < maxd && d > mind)
    d = sdf((p += 0.5*d*dir));
  return p;
}

vec3 sdfnormal(vec3 p){
  float d = 0.01;
  float gx = (sdf(p + d*vec3(1,0,0)) - sdf(p - d*vec3(1,0,0)))/(2*d);
  float gy = (sdf(p + d*vec3(0,1,0)) - sdf(p - d*vec3(0,1,0)))/(2*d);
  float gz = (sdf(p + d*vec3(0,0,1)) - sdf(p - d*vec3(0,0,1)))/(2*d);
  return normalize(vec3(gx, gy, gz));
}

vec4 light(vec3 p){
  vec3 n = sdfnormal(p);
  float diffuse = 0.5*dot(n, lightpos);
  float specular = 0.1*pow(max(0.0, dot(reflect(normalize(lightpos), n), normalize(p - camerapos))), 64);
  float ambient = 1.0;
  return vec4(vec3(diffuse+specular+ambient)*lightcol, 1.0);
}

float shade(vec3 p){
  vec3 dir = normalize(p - lightpos);
  vec3 e = ray(lightpos, dir, 0.01, length(p-lightpos));
  if(length(e - p) < 0.1) return 1.0;
  else return 0.9;
}

void main(){
  vec3 dir = normalize(lookpos - camerapos);    //Look Direction
  vec3 nx = normalize(cross(dir, vec3(0,1,0))); //Normal Vector Sideways
  vec3 ny = normalize(cross(dir, nx));          //Normal Vector Up

  vec2 t = (ex_Tex*2-vec2(1.0))*zoom;           //Fragment Position
  vec3 s = camerapos + t.x*nx + t.y*ny;         //Starting Position

  float maxd = 15.0;
  float mind = 0.001;
  vec3 e = ray(s, dir, mind, maxd);
  float d = length(e - s)/maxd;

  if(d < 1.0) fragColor = shade(e)*light(e)*converge;
  else fragColor = diverge;
}

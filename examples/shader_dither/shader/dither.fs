#version 330
in vec2 ex_Tex;
out vec4 fragColor;

uniform sampler2D imageTexture;

uniform int pres;
uniform int cres;

ivec2 res = textureSize(imageTexture,0);

const int MSIZE = 8;
const int IM[MSIZE*MSIZE] = int[](0,  32, 8,  40, 2,  34, 10, 42,
                                     48, 16, 56, 24, 50, 18, 58, 26,
                                     12, 44, 4,  36, 14, 46, 6,  38,
                                     60, 28, 52, 20, 62, 30, 54, 22,
                                     3,  35, 11, 43, 1,  33, 9,  41,
                                     51, 19, 59, 27, 49, 17, 57, 25,
                                     15, 47, 7,  39, 13, 45, 5,  37,
                                     63, 31, 55, 23, 61, 29, 53, 21);

float thresh(vec2 p) {
  ivec2 v = ivec2(mod(floor(p*res/pres),MSIZE));
  return IM[(v.x + v.y * MSIZE)] / float(MSIZE*MSIZE);
}

vec3 dither() {

  vec2 p = floor(ex_Tex*res/pres)/res*pres;         //Pixelize Position

/*
  //Original Color in Field
  vec3 C = vec3(0);
  for(int i = 0; i < pres; i++){
    for(int j = 0; j < pres; j++){
      C += texture(imageTexture, p+ivec2(i,j)/res).xyz;
    }
  }
  C/=pres*pres;
*/

  vec3 C = texture(imageTexture, p).xyz;

  //Two Nearest Palette Colors
  vec3 A = round(C*cres);
  vec3 B = A;
  vec3 D = mod(C*cres,1.0);
  B.x = (D.x > 0.5)?floor(C.x*cres):ceil(C.x*cres);
  B.y = (D.y > 0.5)?floor(C.y*cres):ceil(C.y*cres);
  B.z = (D.z > 0.5)?floor(C.z*cres):ceil(C.z*cres);

  //Ordered Thresholding
  float d = length(A/cres-C)/length(B/cres-C);
  return (d < thresh(p)) ? A/cres : B/cres;

}

void main () {
  fragColor = vec4(dither(), 1); //Return Dither
}

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout (std430, binding = 0) buffer positionA {
  vec4 pA[];
};

layout (std430, binding = 1) buffer positionB {
  vec4 pB[];
};

layout (std430, binding = 2) buffer velocityA {
  vec4 vA[];
};

layout (std430, binding = 3) buffer velocityB {
  vec4 vB[];
};

uniform bool collide;
uniform int size;
uniform float R;

const float dt = 0.00005f;
const float RATIO = 8.0/4.0;
const float G = 10.0f;
const vec2 scale = vec2(1.0, RATIO);

void bcollide(uint index, vec2 b, float bR){

  float dist = length(b/scale-pB[index].xy/scale);

  if(dist > R + bR)
    return;

  vec2 nv = vA[index].xy/scale - 2*dot(vA[index].xy/scale, pA[index].xy/scale-b/scale)/dot(pA[index].xy/scale-b/scale, pA[index].xy/scale-b/scale)*(pA[index].xy/scale-b/scale);
  vB[index].xy = scale*nv;

  vec2 dir = normalize(b/scale-pA[index].xy/scale);
  pB[index].xy = scale*(b/scale - (bR+R+1E-8)*dir);

}

//void ncollide(uint index, vec2 )

void main() {

  const uvec3 wsize = gl_WorkGroupSize*gl_NumWorkGroups;
  const uint index = gl_GlobalInvocationID.x*wsize.y+gl_GlobalInvocationID.y;

  if(index > size) return;

  if(collide){

    vB[index] = vA[index];
    pB[index] = pA[index];


    // Boundary Collision Collide

    for(int i = 0; i < size; i++){

      if(i == index)
        continue;

      // If two guys are a specific distance from each other, then have them collide

      float dist = length(pA[i].xy/scale-pA[index].xy/scale);

      if(dist > 2*R)
        continue;

      // Compute the New Direction

      vec2 nv = vA[index].xy/scale - dot(vA[index].xy/scale-vA[i].xy/scale,pA[index].xy/scale-pA[i].xy/scale)/dot(pA[index].xy/scale-pA[i].xy/scale, pA[index].xy/scale-pA[i].xy/scale)*(pA[index].xy/scale-pA[i].xy/scale);
      vB[index].xy = scale*nv;

      vec2 dir = pA[i].xy/scale-pA[index].xy/scale;
      vec2 mid = 0.5*(pA[i].xy/scale+pA[index].xy/scale);

      pB[index].xy -= scale*(2*R-length(dir))*normalize(dir);

    }

    bcollide(index, vec2(0.5, 0), 0.15);
    bcollide(index, vec2(-0.5, 0), 0.15);
    bcollide(index, vec2(0,0.5), 0.15);
    bcollide(index, vec2(0,-0.5), 0.15);

  }

  if(!collide){

    // Boundary Condition

    vA[index] = vB[index];
    pA[index] = pB[index];

    /*

    if(p[index].x < -1.0 + R && v[index].x < 0.0){
      v[index].x *= -1.0;
      p[index].x = ( -1.0 + R ) - (1.0 + p[index].x) + R;
    }

    if(p[index].x > 1.0 - R && v[index].x > 0.0){
      v[index].x *= -1.0;
      p[index].x = ( 1.0 - R ) + (1.0 - p[index].x) - R;
    }

    */
        /*
        if(pA[index].y < -1.0 + R*RATIO && vA[index].y < 0.0){
          vA[index].y *= -1.0;
          pA[index].y = ( -1.0 + R*RATIO ) - (1.0 + pA[index].y) + R*RATIO;
        }

        if(pA[index].y > 1.0 - R*RATIO && vA[index].y > 0.0){
          vA[index].y *= -1.0;
          pA[index].y = ( 1.0 - R*RATIO ) + (1.0 - pA[index].y) - R*RATIO;
        }
        */





    if(pA[index].y < -1.0){
      pA[index].y = 1.0 + (pA[index].y + 1.0);
    }

    if(pA[index].y > 1.0){
      pA[index].y = -1.0 + (pA[index].y - 1.0);
    }


    if(pA[index].x > 1.0){
      pA[index].x = -1.0 + (pA[index].x - 1.0);
      pA[index].y = -1.0 + 2.0*float(index%100)/100.0f;
    }

    if(pA[index].x < -1.0){
      pA[index].x = 1.0 + (pA[index].x + 1.0);
    }

    /*



    // Periodic Boundary Condition



    */

    pA[index] += dt*vA[index];

    vec2 vv = vA[index].xy;

    vv *= 0.995f;
    vv += dt * 20000.0 * vec2(1, 0);// * length(vec2(1, 0) - vv);

    vA[index].xy = vv;
    return;

  }

};

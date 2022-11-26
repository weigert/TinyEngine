/*

  Cloth Force Computation

*/

vec4 nv, np;
const float k = 5; // Elasticity
const float damp = 0.02;
const float mass = 1.0f;
const float shear = 10;
const float flatten = 0.0;

vec4 cforce( uint index, int rk ){

  const int ind_x = int(index / res);
  const int ind_y = int(index % res);

  vec4 f = vec4(0);

  // Tension Force between Nodes

  vec4 dir;
  float scale;

  if(ind_x+1 < res){
    dir = normalize(p[rk*size + (ind_x+1) * res + ind_y] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x+1) * res + ind_y] - p[rk*size + index]) - 1;
    f += k*scale*dir;
  }

  if(ind_x-1 >= 0){
    dir =  normalize(p[rk*size + (ind_x-1) * res + ind_y] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x-1) * res + ind_y] - p[rk*size + index]) - 1;
    f += k*scale*dir;
  }

  if(ind_y+1 < res){
    dir =  normalize(p[rk*size + (ind_x) * res + ind_y+1] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x) * res + ind_y+1] - p[rk*size + index]) - 1;
    f += k*scale*dir;
  }

  if(ind_y-1 >= 0){
    dir =  normalize(p[rk*size + (ind_x) * res + ind_y-1] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x) * res + ind_y-1] - p[rk*size + index]) - 1;
    f += k*scale*dir;
  }


  // Flatten

  if(ind_x-1 >= 0 && ind_x+1 < res){

    vec4 m = 0.5*(p[rk*size + (ind_x-1) * res + ind_y] + p[rk*size + (ind_x+1) * res + ind_y]);
    dir = m - p[rk*size + index];
    f += flatten*dir;

  }

  if(ind_y-1 >= 0 && ind_y+1 < res){

    vec4 m = 0.5*(p[rk*size + (ind_x) * res + ind_y-1] + p[rk*size + (ind_x) * res + ind_y+1]);
    dir = m - p[rk*size + index];
    f += flatten*dir;

  }



  // Shearing

  if(ind_x-1 >= 0 && ind_y-1 >= 0){

    dir = normalize(p[rk*size + (ind_x-1) * res + ind_y-1] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x-1) * res + ind_y-1] - p[rk*size + index]) - sqrt(2);
    f += shear*scale*dir;

  }

  if(ind_x+1 < res && ind_y+1 < res){

    dir = normalize(p[rk*size + (ind_x+1) * res + ind_y+1] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x+1) * res + ind_y+1] - p[rk*size + index]) - sqrt(2);
    f += shear*scale*dir;

  }

  if(ind_x+1 < res && ind_y-1 >= 0){

    dir = normalize(p[rk*size + (ind_x+1) * res + ind_y-1] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x+1) * res + ind_y-1] - p[rk*size + index]) - sqrt(2);
    f += shear*scale*dir;

  }

  if(ind_x-1 >= 0 && ind_y+1 < res){

    dir = normalize(p[rk*size + (ind_x-1) * res + ind_y+1] - p[rk*size + index]);
    scale = length(p[rk*size + (ind_x-1) * res + ind_y+1] - p[rk*size + index]) - sqrt(2);
    f += shear*scale*dir;

  }

  // Damp with Velocity Vector

  f -= damp*v[rk*size + index];

  // Add Gravity

  f += 0.005*vec4(0,-1,0,0);

//  vec3 normal = cross((p[index] - p[(ind_x-1) * res + ind_y+1]).xyz, (p[index] - p[(ind_x) * res + ind_y+1]).xyz);
//  f += abs(dot(vec3(0,0,1), normal))*0.005*vec4(0,0,1,0)*sin(0.0005*t)*sin(0.0005*t);

  f.w = 0.0f;

  return f;

}

void shift( uint index ){

  /*

  f[3*size + index] = f[2*size + index];
  v[3*size + index] = v[2*size + index];
  p[3*size + index] = p[2*size + index];

  f[2*size + index] = f[1*size + index];
  v[2*size + index] = v[1*size + index];
  p[2*size + index] = p[1*size + index];

  f[1*size + index] = f[0*size + index];
  v[1*size + index] = v[0*size + index];
  p[1*size + index] = p[0*size + index];
  */

  p[index] = np;
  v[index] = nv;

}

void leapfrog( uint index ){

  v[index] = v[index] + dt*f[index]/mass;
  p[index] = p[index] + dt*v[index];

}

void symplectic1( uint index ){

  nv = v[index] + dt*f[index]/mass;
  np = p[index] + dt*nv;

  shift(index);

}

void symplectic2( uint index ){

  nv = v[index] + dt*f[index]/mass;
  np = p[index] + dt*nv;

  shift(index);

}

void verlet( uint index ){

  np = 2.0f*p[index] - p[size + index] + dt*dt*f[index]/mass;
  nv = (np - p[size + index])/(2*dt);

  //nv = (3.0*np - 4.0*p[index] + p[size+index])/(2.0*dt);

  shift(index);

}



void RK4( uint index, int rk ){

  if( rk == 0 ){

    v[1*size + index] = v[index] + 0.5*dt*f[0*size+index];
    p[1*size + index] = p[index] + 0.5*dt*v[0*size+index];

  }

  if( rk == 1 ){

    v[2*size + index] = v[index] + 0.5*dt*f[1*size+index];
    p[2*size + index] = p[index] + 0.5*dt*v[1*size+index];

  }

  if( rk == 2 ){

    v[3*size + index] = v[index] + dt*f[2*size+index];
    p[3*size + index] = p[index] + dt*v[2*size+index];

  }

  if( rk == 3 ){

    v[index] = v[index] + dt/6.0*(f[index] + 2*f[size+index] + 2*f[2*size+index] + f[3*size+index]);
    p[index] = p[index] + dt/6.0*(v[index] + 2*v[size+index] + 2*v[2*size+index] + v[3*size+index]);

  }

}

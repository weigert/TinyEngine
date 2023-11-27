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

  f += G*vec4(0,-1,0,0)*mass;

//  vec3 normal = cross((p[index] - p[(ind_x-1) * res + ind_y+1]).xyz, (p[index] - p[(ind_x) * res + ind_y+1]).xyz);
//  f += abs(dot(vec3(0,0,1), normal))*0.005*vec4(0,0,1,0)*sin(0.0005*t)*sin(0.0005*t);

  f.w = 0.0f;

  return f;

}
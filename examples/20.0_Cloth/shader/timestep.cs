void shift( uint index ){

  /*

  f[3*size + index] = f[2*size + index];
  v[3*size + index] = v[2*size + index];
  p[3*size + index] = p[2*size + index];

  f[2*size + index] = f[1*size + index];
  v[2*size + index] = v[1*size + index];
  p[2*size + index] = p[1*size + index];
  */

  v[1*size + index] = v[0*size + index];
  p[1*size + index] = p[0*size + index];

  p[index] = np;
  v[index] = nv;

}

void leapfrog( uint index ){

  nv = v[index] + dt*f[index]/mass;
  np = p[index] + dt*v[index];

  shift(index);

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

#version 460 core

layout(local_size_x = 1024) in;

layout (std430, binding = 2) buffer probability {
  float P[];
};

layout (std430, binding = 3) buffer vector {
  float V[];
};

uniform int operation;

uniform float set;
uniform bool minordim;

uniform int DX;
uniform int DY;

void main(){

  uint index = gl_GlobalInvocationID.x;

  //Sum Matrix Along Dimension into Vector
  if(operation == 0){

    V[index] = set;

    if(minordim)
      for(int i = 0; i < DX; i++)
        V[index] += P[i*DY+index];

    else
      for(int i = 0; i < DY; i++)
        V[index] += P[index*DY+i];

  }

  if(operation == 1){

    for(int i = 0; i < DX; i++)
      P[i*DY+index] /= V[index];

  }

}

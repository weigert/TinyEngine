uniform int size;
uniform int res;
uniform int t;
uniform int mode;
uniform int rk;

const float dt = 0.01f;
const float G = 0.1f;

vec4 nv, np;
const float k = 10.0; // Elasticity
const float damp = 0.1;
const float mass = 0.1f;
const float shear = 2.0;
const float flatten = 0.01;

#include <glm/glm.hpp>
#include <vector>

namespace color {
using namespace glm;

vec3 black = vec3(0.0);
vec3 white = vec3(1.0);

template <typename T>
T bezier(float t, std::vector<T> points){
  while(points.size() > 1){ //Recursive Linear Interpolations
    std::vector<T> next;
    for(unsigned int i = 0; i < points.size()-1; i++)
      next.push_back( (1.0f-t)*points[i] + t*points[i+1] );
    points = next;
  }
  return points[0];
}

template <typename T>
T bezier(float t, std::initializer_list<T> p){
  std::vector<T> points;
  points.insert(points.end(), p.begin(), p.end());
  return bezier(t, points);
}

vec3 pick(int i, const int N){
  int Z = (float)((i%N));
  int Y = (float)((i/N)%N);
  int X = (float)((i/(N*N))%N);
  return vec3(X, Y, Z)/vec3(N-1.0f);
}

template<typename T>
int index(T c, const int N){
  vec3 w = vec3(N-1.0f)*vec3(N*N, N, 1);
  return c.x*w.x + c.y*w.y + c.z*w.z;
}

vec3 rainbow(float t){
  return bezier(t, {vec3(1,0,0), vec3(1,1,0), vec3(0,1,0), vec3(0,1,1), vec3(0,0,1), vec3(1,0,1)});
}

vec4 i2rgba(int i){
  return vec4(((i >>  0) & 0xff),((i >>  8) & 0xff),((i >> 16) & 0xff),((i >> 24) & 0xff));
}


vec3 i2rgb(int i){
  return vec3(((i >>  0) & 0xff),((i >>  8) & 0xff),((i >> 16) & 0xff));
}

//Use the Position Hash (Generates Hash from Index)
std::hash<std::string> position_hash;
double hashrand(int i){
  return (double)(position_hash(std::to_string(i))%1000)/1000.0;
}

}

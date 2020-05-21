namespace color{
  glm::vec3 black = glm::vec3(0.0);
  glm::vec3 white = glm::vec3(1.0);

  template <typename T>
  T bezier(float t, std::vector<T> points){
    while(points.size() > 1){ //Recursive Linear Interpolations
      std::vector<T> next;
      for(unsigned int i = 0; i < points.size()-1; i++)
        next.push_back( (1.0-t)*points[i] + t*points[i+1] );
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

  glm::vec3 pick(int i, const int N){
    int Z = (float)((i%N));
    int Y = (float)((i/N)%N);
    int X = (float)((i/(N*N))%N);
    return glm::vec3(X, Y, Z)/glm::vec3(N-1.0f);
  }

  template<typename T>
  int index(T c, const int N){
    glm::vec3 w = glm::vec3(N-1.0f)*glm::vec3(N*N, N, 1);
    return c.x*w.x + c.y*w.y + c.z*w.z;
  }
}

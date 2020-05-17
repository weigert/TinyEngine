using colorscheme = std::vector<glm::vec4>;

namespace color{

  glm::vec3 black = glm::vec3(0.0);
  glm::vec3 white = glm::vec3(1.0);

  //Generate hash from 0-1 from integer
  std::hash<std::string> position_hash;
  double hashrand(int i){
    return (double)(position_hash(std::to_string(i))%1000)/1000.0;
  }

  //De-Casteljau Algorithm (Templated)
  template <typename T>
  T bezier(double t, std::vector<T> points){
    while(points.size() > 1){ //Recursive Linear Interpolations
      std::vector<T> next;
      for(unsigned int i = 0; i < points.size()-1; i++)
        next.push_back( (1.0-t)*points[i] + t*points[i+1] );
      points = next;
    }
    return points[0];
  }

  template <typename T>
  T bezier(double t, std::initializer_list<T> p){
    std::vector<T> points;
    points.insert(points.end(), p.begin(), p.end());
    while(points.size() > 1){ //Recursive Linear Interpolations
      std::vector<T> next;
      for(unsigned int i = 0; i < points.size()-1; i++)
        next.push_back( (1.0-t)*points[i] + t*points[i+1] );
      points = next;
    }
    return points[0];
  }
}

glm::vec4 operator*(const double &d, const glm::vec4& vec) {
  return glm::vec4(d)*vec;
}

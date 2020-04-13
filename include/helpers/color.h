using colorscheme = std::vector<glm::vec4>;

namespace color{
  colorscheme skycolors{
    glm::vec4(0.1, 0.1, 0.1, 1.0),
    glm::vec4(0.00, 1.00, 1.00, 1.0),
    glm::vec4(1.00, 1.00, 1.00, 1.0),
    glm::vec4(0.1, 0.1, 0.1, 1.0)
  };

  colorscheme sandcolors{
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0),
    glm::vec4(0.93, 0.91, 0.38, 1.0)
  };

  colorscheme grasscolors{
    glm::vec4(0.65, 0.72, 0.34, 1.0),
    glm::vec4(0.19, 0.45, 0.34, 1.0),
    glm::vec4(0.54, 0.70, 0.34, 1.0)
  };

  colorscheme leafcolors{
    glm::vec4(0.17, 0.40, 0.26, 0.8),
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0)
  };

  colorscheme claycolors{
    glm::vec4(0.97f, 0.5f, 0.44f, 1.0f),
    glm::vec4(0.75, 0.30, 0.30, 1.0)
  };

  //Pre-Defined Colors
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
      for(int i = 0; i < points.size()-1; i++)
        next.push_back( (1.0-t)*points[i] + t*points[i+1] );
      points = next;
    }
    return points[0];
  }
}

//Overloaded Operator for De-Casteljau Algorithm
glm::vec4 operator*(const double &d, const glm::vec4& vec) {
  return glm::vec4(d)*vec;
}

/*
-> Function to Generate Bezier Coefficients (as vectors)
-> Function to quickly evaluate polynomials
-> Function to to convert HSV to RGB and vice versa

Can we discuss why one thing or the other is faster?



*/

/*
  High order curves lack numerical stability because
  of the evaluation of the inverse of a factorial - this can become very large
  and thereby give you inaccurate results.
  These ratios of factorials are not good. It provides an in-between step that moves over
  vastly different orders of magnitude, which causes errors in the machine precision.
*/

namespace color{
  std::vector<glm::vec4> skycolors{
    glm::vec4(0.1, 0.1, 0.1, 1.0),            //Dark Grey
  //  glm::vec4(1.00, 0.00, 0.00, 1.0),         //Red
    glm::vec4(0.00, 1.00, 1.00, 1.0),         //Blue
    glm::vec4(1.00, 1.00, 1.00, 1.0),         //White
  //  glm::vec4(0.20, 0.67, 0.90, 1.0),         //Dark Blue
  //  glm::vec4(0.8, 0.20, 0.80, 1.0),         //Red
    glm::vec4(0.1, 0.1, 0.1, 1.0)             //Dark Grey
  };

  //Get some Sand-Colors for the Bezier Curve
  std::vector<glm::vec4> sandcolors{
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0),
    glm::vec4(0.93, 0.91, 0.38, 1.0)
  };

  std::vector<glm::vec4> grasscolors{
    glm::vec4(0.65, 0.72, 0.34, 1.0),
    glm::vec4(0.19, 0.45, 0.34, 1.0),
    glm::vec4(0.54, 0.70, 0.34, 1.0)
  };

  std::vector<glm::vec4> leafcolors{
    glm::vec4(0.17, 0.40, 0.26, 0.8),
    glm::vec4(1.00, 0.75, 0.35, 1.0),
    glm::vec4(0.84, 0.21, 0.21, 1.0)
  };

  std::vector<glm::vec4> claycolors{
    glm::vec4(0.97f, 0.5f, 0.44f, 1.0f),
    glm::vec4(0.75, 0.30, 0.30, 1.0),
  //  glm::vec4(0.9, 0.65, 0.28, 1.0)
  };

  //Multiply Color by Constant Factor
  glm::vec4 operator*(const double &d, const glm::vec4& vec) {
    return glm::vec4(d)*vec;
  }

  //Use the Position Hash (Generates Hash from Index)
  std::hash<std::string> position_hash;
  double hashrand(int i){
    return (double)(position_hash(std::to_string(i))%1000)/1000.0;
  }

  //De-Casteljau Algorithm (Templated)
  template <typename T>
  T bezier(double t, std::vector<T> points){
    //Recursively do Linear Interpolations
    while(points.size() > 1){
      std::vector<T> next;
      for(int i = 0; i < points.size()-1; i++)
        next.push_back( (1.0-t)*points[i] + t*points[i+1] );
      points = next;
    }
    return points[0];
  }
}

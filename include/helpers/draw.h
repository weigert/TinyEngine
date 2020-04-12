/*
  Basically: We make a surface from an array by passing it through some function that generates
  the color of the surface. The draw helper provides a number of methods to do just that.
  Then we can generate a picture from raw array data.
*/

namespace draw{

  //Black / White
  std::function<glm::vec4(double)> BW = [](double t){
    return glm::vec4(t, t, t, 1.0);
  };

  //Bezier with Color Scheme!
  std::function<glm::vec4(double)> grass = [](double t){
    return color::bezier<glm::vec4>(t, color::grasscolors);
  };

}; //End of Namespace

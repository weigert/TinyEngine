/*
Mathematical Easing Helpers!

Numbers between [0, 1] are mapped to [0, 1].
*/

namespace ease{

  //Quadratic (0.0 => 0.0, 0.5 => 1.0, 1.0 => 0.0)
  template<typename T>
  T quadratic(T in){
    T out = 4*in*(1 -in);
    return out;
  };

  //Cubic (0.0 => 0.0, 0.5 => 0.5, 1.0 => 1.0)
  template<typename T>
  T cubic(T in){
    T out = in*(3 +in*(-6 + 4*in));
    return out;
  };

  //Quartic (0.0 => 0.0, 0.5 => 1.0, 1.0 => 0.0)
  template<typename T>
  T quartic(T in){
    T out = 8*in*(1 +in*(-3 +in*(4 -2*in)));
    return out;
  };

}

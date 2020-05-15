/*
    Basic Command-Line Arguments Extractor
*/

namespace parse{

  //Ignore the program name as an argumens
  bool ignoreprog = true;

  /* Recursive Argument Identifier */

  template <typename T>
  bool arg(char* a, T& t){
    std::cout<<"Parse data type not recognized"<<std::endl;
    return false;
  };

  template <typename T>
  bool input(int n, char* as[], T& m){
      if(n == ignoreprog) return false;
      if(!arg(as[ignoreprog], m)) return false;
      return true;
  };

  template <typename T, typename... Types>
  bool input(int n, char* as[], T& m, Types&... r){
      if(n == ignoreprog) return false;
      if(!arg(as[ignoreprog], m)) return false;
      return input(n-1, &as[1], r...);
  };

  /* Templated Data-Type Extractors (define more if you like) */

  template<>
  bool arg(char* a, int& i){
    i = atoi(a);
    return true;
  }

  template<>
  bool arg(char* a, std::string& s){
    s = a;
    return true;
  }

  template<>
  bool arg(char* a, float& f){
    f = atof(a);
    return true;
  }

  template<>
  bool arg(char* a, double& d){
    d = atof(a);
    return true;
  }

  template<>
  bool arg(char* a, short& s){
    s = atoi(a);
    return true;
  }

};

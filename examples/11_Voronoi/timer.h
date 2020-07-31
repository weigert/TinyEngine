#include <chrono>

namespace timer{

  //Benchmarking Helper
  template<typename D, typename F, typename... Args>
  void benchmark(F function, Args&&... args){
    auto start = std::chrono::high_resolution_clock::now();
    function(args...);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<D>(stop - start);
    std::cout<<"Execution Time: "<<duration.count()<<std::endl;
  };

}

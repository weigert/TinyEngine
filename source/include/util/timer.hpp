#ifndef TINYENGINE_UTIL_TIMER
#define TINYENGINE_UTIL_TIMER

#include <chrono>
#include <thread>
#include <atomic>

namespace Tiny {
namespace timer {

typedef std::chrono::milliseconds ms;

template<typename D, typename F, typename... Args>
float benchmark(F function, Args&&... args, bool out = true){
  auto start = std::chrono::high_resolution_clock::now();
  function(args...);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<D>(stop - start);
  if(out) std::cout<<"Execution Time: "<<duration.count()<<std::endl;
  return duration.count();
};

template<typename D>
struct measure {
  measure(){
    start = std::chrono::high_resolution_clock::now();
  }
  ~measure(){
    stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<D>(stop - start);
    std::cout<<"Execution Time: "<<duration.count()<<std::endl;
  }
  std::chrono::time_point<std::chrono::high_resolution_clock> start;
  std::chrono::time_point<std::chrono::high_resolution_clock> stop;
};

} // end of namespace timer
} // end of namespace Tiny

#endif

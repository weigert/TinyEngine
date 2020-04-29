#include <chrono>
#include <thread>

namespace timer{

  template<typename D, typename F, typename... Args>
  void benchmark(F function, Args&&... args){
    auto start = std::chrono::high_resolution_clock::now();
    function(args...);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<D>(stop - start);
    std::cout<<"Execution Time: "<<duration.count()<<std::endl;
  };

  /* Example Usage:
        timer::benchmark<std::chrono::microseconds>([&](){
            //Your Code Here
        });
  */

  //Timer Class - Execute code on a timed cycle
  template<typename D>
  class Timer{
    std::atomic<bool> active = false;

    std::chrono::time_point<std::chrono::high_resolution_clock> curTime;
    D delayTime = D(0);

    std::thread t;

  public:

    //Timeout Setter
    template<typename F>
    void set_timeout(D duration, F function){
      active = true;
      t = std::thread([=](){
        std::this_thread::sleep_for(duration);
        if(!active) return; //Maybe the Thread was terminated before the execution of function
        function();
        return;
      });
      t.detach();
    };

    //Interval Setter
    template<typename F, typename... Args>
    void set_interval(D* duration, F function, Args&&... args){
      active = true;
      t = std::thread([=](){
        while(active){
          std::this_thread::sleep_for(*duration);
          function(args...);
        }
        return;
      });
      t.detach();
    };

    //Interval Setter
    template<typename F, typename... Args>
    void set_const_interval(D* duration, F function, Args&&... args){
      active = true;
      t = std::thread([=](){
        while(active){
          std::this_thread::sleep_for(*duration-delayTime);
          curTime = std::chrono::high_resolution_clock::now();
          function(args...);
          delayTime = std::chrono::duration_cast<D>(std::chrono::high_resolution_clock::now() - curTime);
        }
        return;
      });
      t.detach();
    };

    //Stopper
    void stop(){
      //Stop the Process and Join the Thread
      active = false;
      if(t.joinable()){
        t.join();
      }
    }
  };
}

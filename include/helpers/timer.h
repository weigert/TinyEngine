namespace timer{
using namespace std::chrono;

  //Benchmark Code directly in a lambda
  template<typename D, typename F, typename... Args>
  void benchmark(F function, Args&&... args){
    auto start = high_resolution_clock::now();
    function(args...);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<D>(stop - start);
    std::cout<<"Execution Time: "<<duration.count()<<std::endl;
  };

  /* Example Usage:
        timer::benchmark<std::chrono::microseconds>([&](){
            //Your Code Here
        });
  */

  //Time Points
  bool started = false;
  time_point<high_resolution_clock> A;
  time_point<high_resolution_clock> B;
  bool start(){
    if(started) return false;
    A = high_resolution_clock::now();
    started = true;
    return true;
  }
  bool stop(){
    if(!started) return false;
    B = high_resolution_clock::now();
    started = false;
    return true;
  }
  template<typename D>
  int time(){
    return duration_cast<D>(B-A).count();
  }

  //Timer Class - Execute code on a timed cycle
  template<typename D>
  class Timer{
    std::atomic<bool> active = false;

    time_point<high_resolution_clock> curTime;
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
          curTime = high_resolution_clock::now();
          function(args...);
          delayTime = duration_cast<D>(high_resolution_clock::now() - curTime);
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

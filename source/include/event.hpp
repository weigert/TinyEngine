#ifndef TINYENGINE_EVENT
#define TINYENGINE_EVENT

#include <TinyEngine/View>

#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace Tiny {

//! \brief Dispatch is a Callback Set Container
//! This type contains a set of callbacks with no parameters,
//! which are intended to be collectively executed when an event occurs.
//! Note that callbacks cannot be removed from this container.
struct dispatch {

  typedef std::function<void()> callback_t;

  //! Move a Callback into the Container
  //! Uses std::forward for lambda insertion.
  template<typename F>
  void operator()(F&& callback){
    callbacks.emplace_back(std::forward<F>(callback));
  }

  //! Dispatch the Set of Callbacks
  void operator()(){
    for(auto& callback: callbacks)
      callback();
  }

private:
  std::vector<callback_t> callbacks = {};
};

//! \brief Dispatch is a Callback Set Container (w. Parameter)
//! This type contains a set of callbacks with a single parameter, 
//! which are intended to be collectively executed when an event occurs.
//! Note that callbacks cannot be removed from this container.
template<typename T>
struct dispatch_p {

  typedef T param_t;                                //!< Callback Parameter 
  typedef std::function<void(param_t)> callback_t;  //!< Callback Type

  //! Move a Callback into the Container
  //! Uses std::forward for lambda insertion.
  template<typename F>
  void operator()(F&& callback){
    callbacks.emplace_back(std::forward<F>(callback));
  }

  //! Dispatch the Set of Callbacks
  void operator()(param_t param){
    for(auto& callback: callbacks)
      callback(param);
  }

private:
  std::vector<callback_t> callbacks = {};
};

/*
// NOTE: ADD THIS BACK IN FOR UNHOOKING!

//! \brief Dispatch Map Container
//! This type contains a map of dispatchers, with a uniform parameter type.
//! The map of dispatchers allows for choosing a set of callbacks to execute
//! based on a key type, with a convenient operator interface.
template<typename K, typename T>
struct dispatch_set {

  typedef K key_t;                //!< Type of Dispatch Indexing Key
  typedef dispatch<T> dispatch_t; //!< Type of Parameterized Callback Set

  dispatch_t& operator[](key_t key){
    return this->_dispatch[key];
  }

private:
  std::unordered_map<K, dispatch_t> _dispatch;
};
*/

struct Event {

  bool quit = false;

  void retrieve();  // Retrieve Events
  void process();   // Process Events

  template<typename T>
  using dset = std::unordered_map<SDL_Keycode, T>;

  dset<dispatch> active;
  dset<dispatch_p<bool>> press;
  dispatch finalize;
  dispatch_p<glm::ivec2> resize;
  dispatch_p<glm::ivec2> scroll;
  
  /*
  // dispatch_p<glm::ivec2> click;
  //Movement Events
  bool mousemove = false;
  SDL_MouseMotionEvent mouse;

  //Clicking Events
  std::unordered_map<Uint8, bool> click;      //Active Buttons
  std::deque<Uint8> clicked;                  //Button Events
  */

private:
  std::unordered_set<SDL_Keycode> active_set;
  std::deque<SDL_KeyboardEvent> press_queue;
  SDL_Event in;
};

}

#endif
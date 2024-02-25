#ifndef TINYENGINE_EVENT
#define TINYENGINE_EVENT

#include <TinyEngine/View>

#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <functional>

namespace Tiny {

template<typename T>
struct dispatch_base {

  typedef std::function<T> callback_t;

  template<typename F>
  void operator()(F&& callback){
    callbacks.emplace_back(std::forward<F>(callback));
  }

protected:
  std::vector<callback_t> callbacks = {};
};

//! \brief Dispatch is a Callback Set Container
//! This type contains a set of callbacks with no parameters,
//! which are intended to be collectively executed when an event occurs.
//! Note that callbacks cannot be removed from this container.
template<typename P>
struct dispatch: dispatch_base<void(P)> {
  
  using dispatch_base<void(P)>::operator();

  //! Dispatch the Set of Callbacks
  void operator()(P param){
    for(auto& callback: this->callbacks)
      callback(param);
  }
};

//! No-Arg Specialization
template<>
struct dispatch<void>: dispatch_base<void()> {

  using dispatch_base<void()>::operator();

  //! Dispatch the Set of Callbacks (No Arg)
  void operator()(){
    for(auto& callback: this->callbacks)
      callback();
  }
};

struct mouse_t {
  glm::ivec2 pos;
  glm::ivec2 dir;
  bool left;
  bool middle;
  bool right;
};

struct Event {

  void process();   // Process Events

  template<typename T>
  using dset = std::unordered_map<SDL_Keycode, T>;

  dispatch<SDL_Event*> raw;     //!< Raw Event Processing

  dset<dispatch<void>> active;  //!< Dispatch Set for Actively Pressed Keys
  dset<dispatch<bool>> press;   //!< Dispatch Set for Key Presses / Releases

  dispatch<mouse_t> mouse;      //!< Callback Set for Mouse Move Events
  dispatch<mouse_t> click;      //!< Callback Set for Mouse Click Events

  dispatch<glm::ivec2> scroll;  //!< Callback Set for Mouse Scroll Events
  dispatch<glm::ivec2> resize;  //!< Callback Set for Window Resize Events

  dispatch<void> tick;          //!< Callback Set Per-Tick
  dispatch<void> quit;          //!< Callback Set On Quit

private:
  std::unordered_set<SDL_Keycode> active_set;
  std::deque<SDL_KeyboardEvent> press_queue;
};

}

#endif
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
template<typename P>
struct dispatch {
  
  typedef P param_t;
  typedef std::function<void(P)> callback_t;

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

template<>
struct dispatch<void> {
  
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

struct mouse_t {
  glm::ivec2 pos;
  glm::ivec2 dir;
  bool left;
  bool middle;
  bool right;
};

struct Event {

  void retrieve();  // Retrieve Events
  void process();   // Process Events

  template<typename T>
  using dset = std::unordered_map<SDL_Keycode, T>;

  dset<dispatch<void>> active;    //!< Dispatch Set for Actively Pressed Keys
  dset<dispatch<bool>> press; //!< Dispatch Set for Key Presses / Releases

  dispatch<mouse_t> mouse;      //!< Callback Set for Mouse Move Events
  dispatch<mouse_t> click;      //!< Callback Set for Mouse Click Events

  dispatch<glm::ivec2> scroll;  //!< Callback Set for Mouse Scroll Events
  dispatch<glm::ivec2> resize;  //!< Callback Set for Window Resize Events

  dispatch<void> loop;  //!< Callback Set Per-Loop
  dispatch<void> quit;  //!< Callback Set On Quit

private:
  std::unordered_set<SDL_Keycode> active_set;
  std::deque<SDL_KeyboardEvent> press_queue;
};

}

#endif
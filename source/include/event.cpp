#include "event.hpp"
#include <iostream>

namespace Tiny {

void Event::process(){

  SDL_Event in;
  while(SDL_PollEvent(&in)){

    this->raw(&in);

    switch(in.type){
      case SDL_QUIT:
        this->quit();
        break;
      case SDL_KEYDOWN:
        this->active_set.insert(in.key.keysym.sym);
        this->press_queue.push_front(in.key);
        break;
      case SDL_KEYUP:
        this->active_set.erase(in.key.keysym.sym);
        this->press_queue.push_front(in.key);
        break;
      case SDL_MOUSEWHEEL:
        this->scroll(glm::ivec2(in.wheel.x, in.wheel.y));
        break;
      case SDL_MOUSEMOTION:
        this->mouse(mouse_t{
          {in.motion.x, in.motion.y},
          {in.motion.xrel, in.motion.yrel},
          (in.motion.state & SDL_BUTTON_LMASK) != 0,
          (in.motion.state & SDL_BUTTON_MMASK) != 0,
          (in.motion.state & SDL_BUTTON_RMASK) != 0,
        });
        break;
      case SDL_MOUSEBUTTONDOWN:
        this->mouse(mouse_t{
          {in.motion.x, in.motion.y},
          {in.motion.xrel, in.motion.yrel},
          (in.motion.state & SDL_BUTTON_LMASK) != 0,
          (in.motion.state & SDL_BUTTON_MMASK) != 0,
          (in.motion.state & SDL_BUTTON_RMASK) != 0,
        });
        break;
      case SDL_MOUSEBUTTONUP:
        this->mouse(mouse_t{
          {in.motion.x, in.motion.y},
          {in.motion.xrel, in.motion.yrel},
          (in.motion.state & SDL_BUTTON_LMASK) != 0,
          (in.motion.state & SDL_BUTTON_MMASK) != 0,
          (in.motion.state & SDL_BUTTON_RMASK) != 0,
        });
        break;
      case SDL_WINDOWEVENT:
        if(in.window.event == SDL_WINDOWEVENT_RESIZED){
          this->resize(glm::ivec2((int)in.window.data1, (int)in.window.data2));
        }
        break;
      default:
        break;
    }
  }

  // Dispatch the Pressed Button Queue
  while(!press_queue.empty()){
    SDL_KeyboardEvent event = press_queue.back();
    press[event.keysym.sym](event.state == SDL_PRESSED);
    press_queue.pop_back();
  }
  
  // Dispatch all Active-Set Callbacks
  for(auto& key: active_set)
    active[key]();

  // Dispatch Once-Per-Tick Callbacks
  this->tick();

}

}
#include "event.hpp"
#include <iostream>

namespace Tiny {

void Event::retrieve(){

while(SDL_PollEvent(&in)){

  ImGui_ImplSDL2_ProcessEvent(&in);

  switch(in.type){
    case SDL_QUIT:
      quit = true;
      break;
    case SDL_KEYDOWN:
      active_set.insert(in.key.keysym.sym);
      press_queue.push_front(in.key);
      break;
    case SDL_KEYUP:
      active_set.erase(in.key.keysym.sym);
      press_queue.push_front(in.key);
      break;
    case SDL_MOUSEWHEEL:
      scroll(glm::ivec2(in.wheel.x, in.wheel.y));
      break;
    case SDL_MOUSEMOTION:
      //mouse = in.motion;
      //mousemove = true;
      break;
    case SDL_MOUSEBUTTONDOWN:
      //click[in.button.button] = true;
      break;
    case SDL_MOUSEBUTTONUP:
      //click[in.button.button] = false;
      //clicked.push_front(in.button.button);
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
}

void Event::process(){

  // Dispatch the Pressed Button Queue
  while(!press_queue.empty()){
    SDL_KeyboardEvent event = press_queue.back();
    press[event.keysym.sym](event.state == SDL_PRESSED);
    press_queue.pop_back();
  }
  
  // Dispatch all Active-Set Callbacks
  for(auto& key: active_set)
    active[key]();

  //if(!clicked.empty()) clicked.pop_back();  //Reset Event Triggers
  //mousemove = false;

  this->finalize();

}

}
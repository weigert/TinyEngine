#include "event.h"

void Event::input(){
while(SDL_PollEvent(&in)){

  ImGui_ImplSDL2_ProcessEvent(&in);

  switch(in.type){
    case SDL_QUIT:
      quit = true;
      break;
    case SDL_KEYDOWN:
      active[in.key.keysym.sym] = true;
      break;
    case SDL_KEYUP:
      active[in.key.keysym.sym] = false;
      press.push_front(in.key.keysym.sym);
      break;
    case SDL_MOUSEWHEEL:
      scroll.posy = (in.wheel.y > 0.99);
      scroll.negy = (in.wheel.y < -0.99);
      scroll.posx = (in.wheel.x > 0.99);
      scroll.negx = (in.wheel.x < -0.99);
      break;
    case SDL_MOUSEMOTION:
      mouse = in.motion;
      mousemove = true;
      break;
    case SDL_MOUSEBUTTONDOWN:
      click[in.button.button] = true;
      break;
    case SDL_MOUSEBUTTONUP:
      click[in.button.button] = false;
      clicked.push_front(in.button.button);
      break;
    case SDL_WINDOWEVENT:
      if(in.window.event == SDL_WINDOWEVENT_RESIZED){
        windowEvent = in;
        windowEventTrigger = true;
      }
      break;
    default:
      break;
  }
}
}

void Event::handle(View &view){

  if(windowEventTrigger){
    view.WIDTH = windowEvent.window.data1;
    view.HEIGHT = windowEvent.window.data2;
  }

  (handler)();  //Call user-defined handler

  if(!press.empty()){
    if(press.back() == SDLK_ESCAPE) //Toggle interface visibility
      view.showInterface = !view.showInterface;

    if(press.back() == SDLK_F11){   //Toggle fullscreen
      view.fullscreen = !view.fullscreen;
      if(!view.fullscreen) SDL_SetWindowFullscreen(view.gWindow, 0);
      else SDL_SetWindowFullscreen(view.gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    press.pop_back();
  }

  if(!clicked.empty()) clicked.pop_back();  //Reset Event Triggers
  scroll.reset();
  mousemove = false;
  windowEventTrigger = false;
}

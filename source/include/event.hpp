#ifndef TINYENGINE_EVENT
#define TINYENGINE_EVENT

#include <TinyEngine/View>

#include <deque>
#include <unordered_map>
#include <functional>

namespace Tiny {

struct Scroll{
  bool posx, posy, negx, negy;
  void reset(){
    posx = posy = negx = negy = false;
  }
};

struct Event{
  bool quit = false;
  void input();                   //Take inputs and add them to stack

  void handle(View &view);        //General Event Handler
  std::function<void()> handler = [](){};        //User defined event Handler

  //Keyboard Events
  std::unordered_map<SDL_Keycode, bool> active;
  std::deque<SDL_Keycode> press;

  //Movement Events
  bool mousemove = false;
  SDL_MouseMotionEvent mouse;

  //Clicking Events
  std::unordered_map<Uint8, bool> click;      //Active Buttons
  std::deque<Uint8> clicked;                  //Button Events

  Scroll scroll;
  SDL_Event windowEvent;         //Window Resizing Event
  bool windowEventTrigger = false;

private:  
  SDL_Event in;
};

}

#endif

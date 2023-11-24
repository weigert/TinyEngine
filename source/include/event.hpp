#ifndef TINYENGINE_EVENT
#define TINYENGINE_EVENT

struct Scroll{
  bool posx, posy, negx, negy;
  void reset(){
    posx = posy = negx = negy = false;
  }
};

class Event{
private:
  SDL_Event in;

public:
  bool quit = false;
  void input();                   //Take inputs and add them to stack

  void handle(View &view);        //General Event Handler
  Handle handler = [](){};        //User defined event Handler

  bool fullscreenToggle = false;

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
};

#endif

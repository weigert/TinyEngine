class Event{
private:
  SDL_Event in;

public:
  bool quit = false;
  void input();                   //Take inputs and add them to stack

  void handle(View &view);        //General Event Handler
  Handle handler;                 //User defined event Handler

  bool fullscreenToggle = false;

  std::deque<SDL_Event> keys;
  bool keyEventTrigger = false;
  std::deque<SDL_Event> scroll;  //Scrolling Motion Inputs
  SDL_Event windowEvent;         //Window Resizing Event
  bool windowEventTrigger = false;
  SDL_Event mouseEvent;          //Mouse Click Event
  bool mouseEventTrigger = false;
  SDL_Event moveEvent;           //Mouse Movement Events
  bool moveEventTrigger = false;
};

void Event::input(){
  if(SDL_PollEvent(&in) == 0) return;

  if(in.type == SDL_QUIT) quit = true;
  ImGui_ImplSDL2_ProcessEvent(&in);

  if(in.type == SDL_KEYUP){
    if(in.key.keysym.sym == SDLK_F11) fullscreenToggle = true;
    else keys.push_front(in);
    return;
  }

  if(in.type == SDL_MOUSEWHEEL){
    scroll.push_front(in);
    return;
  }

  if(in.type == SDL_MOUSEBUTTONDOWN ||
     in.type == SDL_MOUSEBUTTONUP){
       mouseEvent = in;
       mouseEventTrigger = true;
       return;
  }

  if(in.type == SDL_MOUSEBUTTONDOWN ||
     in.type == SDL_MOUSEBUTTONUP){
       moveEvent = in;
       moveEventTrigger = true;
       return;
  }

  if(in.type == SDL_WINDOWEVENT){
    windowEvent = in;
    windowEventTrigger = true;
    return;
  }
}

void Event::handle(View &view){
  (handler)();  //Call user-defined handler first

  if(fullscreenToggle){
    view.fullscreen = !view.fullscreen; //Toggle Fullscreen!
    if(!view.fullscreen) SDL_SetWindowFullscreen(view.gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    else SDL_SetWindowFullscreen(view.gWindow, 0);
    fullscreenToggle = false;
  }

  if(windowEventTrigger && windowEvent.window.event == SDL_WINDOWEVENT_RESIZED){
    view.WIDTH = windowEvent.window.data1;
    view.HEIGHT = windowEvent.window.data2;
    windowEventTrigger = false;
  }

  if(!keys.empty() && keys.back().key.keysym.sym == SDLK_ESCAPE){
    view.showInterface = !view.showInterface;
  }

  if(!scroll.empty()) scroll.pop_back();
  if(!keys.empty()) keys.pop_back();
}

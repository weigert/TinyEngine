#define TINYENGINE_CORE_BUFFER
#define TINYENGINE_CORE_INSTANCE
#define TINYENGINE_CORE_MODEL
#define TINYENGINE_CORE_SHADER
#define TINYENGINE_CORE_TARGET
#define TINYENGINE_CORE_TEXTURE

#include <TinyEngine/TinyEngine>

#include <SDL2/SDL.h>
#include <iostream>
#include <csignal>

//! TinyEngine's Main Namespace
namespace Tiny {

// Global Variables

View view;          //Window and Interface  (Requires Initialization)
Event event;        //Event Handler
bool kill = false;

void sighandler(int signal){
  event.quit();
}

bool init(){

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
    printf( "SDL could not initialize! Error: %s\n", SDL_GetError() );
    return false;
  }
  std::cout << "SDL Init done\n";
  
  #ifdef TINYENGINE_COMPATIBILITY
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  std::cout << "Comptibility mode ON\n";
  #else
  std::cout << "Comptibility mode OFF\n";
  #endif

  signal(SIGINT, &sighandler);

  if(!view.windowed && !view.init("TinyEngine OpenGL Context", 0, 0)){ //Open a Dummy Window
    std::cout<<"Failed to launch OpenGL Context"<<std::endl;
    return false;
  }

  event.quit([](){
    kill = true;
  });

  return true;

}

bool window(std::string windowName, int width, int height){ //Open a window

  view.windowed = true;

  if(!Tiny::init())
    return false;

  if(!view.init(windowName, width, height)){ //Start the View Class
    std::cout<<"Failed to launch visual interface."<<std::endl;
    return false;
  }

  event.resize([](glm::ivec2 dim){
    view.WIDTH = dim.x;
    view.HEIGHT = dim.y;
  });

  event.press[SDLK_F11]([](bool press){
    if(!press) view.toggle_fullscreen();
  });

  view.initialized(); // Trigger all Initializations w. Require State
  return true;
}

void quit(){
  if(Tiny::view.enabled) view.quit();
  SDL_Quit();
}

}
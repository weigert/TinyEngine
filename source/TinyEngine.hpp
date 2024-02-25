#ifndef TINYENGINE
#define TINYENGINE

// TinyEngine 1.8
// by Nicholas McDonald
// https://github.com/weigert/TinyEngine

#define TINYENGINE_VERSION "1.8"

#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <TinyEngine/Event>
#include <TinyEngine/View>

#include <TinyEngine/Buffer>
#include <TinyEngine/Instance>
#include <TinyEngine/Model>
#include <TinyEngine/Shader>
#include <TinyEngine/Target>
#include <TinyEngine/Texture>

#include <csignal>

//! TinyEngine's Main Namespace
namespace Tiny {

static View view;           //Window and Interface  (Requires Initialization)
static Event event;         //Event Handler
static bool kill = false;

void sighandler(int signal){
  event.quit();
}

bool init(){

  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
    printf( "SDL could not initialize! Error: %s\n", SDL_GetError() );
    return false;
  }

  #ifndef TINYENGINE_COMPATIBILITY
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
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

template<typename F, typename... Args>
void loop(F function, Args&&... args){

  while(!Tiny::kill){

    if(Tiny::view.enabled){
      event.process();  //Call the event-handling system
    }

    function(args...);      //User-defined Game Loop

    if(Tiny::view.enabled){
      view.render();         //Render View
    }

  }

}

}

#endif
/*
  Include this file in your project if you want to use TinyEngine.
*/

#include <functional>
using Handle = std::function<void()>;
#include <initializer_list>
using slist = std::initializer_list<std::string>;

//Interface Dependencies (DearImGUI)
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_sdl.h"
#include "include/imgui/imgui_impl_opengl3.h"

//Drawing Dependencies
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

//File IO
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

//Helpers
#include "include/helpers/image.h"

//Utility Classes for the Engine
#include "include/utility/shader.cpp"
#include "include/utility/billboard.cpp"
#include "include/utility/model.cpp"

#include "include/view.cpp"
#include "include/event.cpp"
#include "include/audio.cpp"

/* TINY ENGINE */

/*
    Working Princple:

      Define a function that acts as the event handler, with access to the member functions.
      Define a function that draws an interface that can access any variables you want.
      Define an attach function that decides what kinds of elements the view has access to.


    To-Do: Setup Models, Billboards and Shaders
*/

namespace Tiny{

//Main Engine Elements
static View view;       //Window and Interface
static Event event;     //Event Handler
static Audio audio;     //Audio Processor

bool init(std::string windowName, int width, int height){
  //Initialize SDL Core
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
    printf( "SDL could not initialize! Error: %s\n", SDL_GetError() );
    return false;
  }

  //Initialize SDL_Image
  if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ){
    printf( "SDL_Image could not initialize! Error: %s\n", IMG_GetError() );
    return false;
  }

  //Initialize SDL_TTF
  TTF_Init();

  //Initialize the View
  if(!view.init(windowName, width, height)){
    std::cout<<"Failed to launch visual interface."<<std::endl;
    return false;
  }

  if(!audio.init()){
    std::cout<<"Failed to launch audio interface."<<std::endl;
		return false;
	}
}

void quit(){
  view.cleanup();
  audio.cleanup();
  TTF_Quit();
  SDL_Quit();
};

//Game Loop
template<typename F, typename... Args>
void loop(F function, Args&&... args){
  //Do the main game loop!
  while(!event.quit){
    //Take user Input
    event.input();
    event.handle(view);

    //Audio Processor
    audio.process();

    //Execute the specified game loop!
    function(args...);

    //Render the View
    view.render();
  }
};

//End of Namespace
};

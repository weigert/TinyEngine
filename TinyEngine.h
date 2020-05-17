#include <functional>
using Handle = std::function<void()>;
#include <initializer_list>
using slist = std::initializer_list<std::string>;

#include <unordered_map>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <termios.h>

#include "include/imgui/imgui.h"                    //Interface Dependencies
#include "include/imgui/imgui_impl_sdl.h"
#include "include/imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>                                //Rendering Dependencies
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include <sstream>                                  //File / Console IO
#include <iostream>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <sys/socket.h>                             //Networking
#include <netinet/in.h>
#include <arpa/inet.h>
#include "include/network/buffer.cpp"
#include "include/network/socket.cpp"

#include "include/helpers/input.h"                  //Helper Namespaces
#include "include/helpers/log.h"
#include "include/helpers/parse.h"
#include "include/helpers/helper.h"
#include "include/helpers/ease.h"
#include "include/helpers/color.h"
#include "include/helpers/image.h"
#include "include/helpers/timer.h"

#include "include/utility/texture.cpp"              //Utility Classes
#include "include/utility/shader.cpp"
#include "include/utility/model.cpp"
#include "include/utility/particle.cpp"
#include "include/utility/target.cpp"

#include "include/helpers/object.h"                 //(Dependency on Model)

#include "include/net.cpp"                          //Main Classes
#include "include/view.cpp"
#include "include/event.cpp"
#include "include/audio.cpp"

/* TINY ENGINE */

namespace Tiny {

//Main Engine Components
static View view;           //Window and Interface  (Requires Initialization)
static Event event;         //Event Handler
static Audio audio;         //Audio Processor       (Requires Initialization)
static Net net;             //Network Handler

bool windowmode = false;    //Just in case you decide to not actually open a window

bool window(std::string windowName, int width, int height){ //Open a window
  windowmode = true;

  //Launch SDL dependencies...
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
    printf( "SDL could not initialize! Error: %s\n", SDL_GetError() );
    return false;
  }

  //Flags necessary to launch OpenGL core profile!
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ){
    printf( "SDL_Image could not initialize! Error: %s\n", IMG_GetError() );
    return false;
  }

  if( TTF_Init() == -1 ){ //for some reason this is -1
    printf( "SDL_ttf could not initialize! Error: %s\n", TTF_GetError() );
    return false;
  }

  if(!view.init(windowName, width, height)){ //Start the View Class
    std::cout<<"Failed to launch visual interface."<<std::endl;
    return false;
  }

  if(!audio.init()){ //Start the Audio Interface
    std::cout<<"Failed to launch audio interface."<<std::endl;
		return false;
	}
  return true;
}

void quit(){
  if(windowmode){
    view.cleanup();  //Cleanup Classes
    audio.cleanup();
    TTF_Quit();      //Stop SDL Dependencies
    SDL_Quit();
  }
  if(net.open)       //Shutdown network interface if openened
    net.close();
};

template<typename F, typename... Args>
void loop(F function, Args&&... args){
  if(windowmode)
    while(!event.quit){
      event.input();        //Get Input
      event.handle(view);   //Call the event-handling system

      if(net.open)
        net.handle();       //Handle Network Messages

      audio.process();      //Audio Processor

      function(args...);    //User-defined Game Loop

      view.render();        //Render View

    }
  else{ //It is possible to just use the network interface without a window
    while(net.open){
      net.handle();        //Handle Network Messages
    }
  }
};

};

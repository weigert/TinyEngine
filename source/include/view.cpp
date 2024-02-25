#include "view.hpp"

namespace Tiny {

bool View::init(std::string _name, int W, int H){
  enabled = windowed;
  WIDTH = W; HEIGHT = H;

  #ifndef TINYENGINE_COMPATIBILITY
  if(antialias)
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, antialias);
  #endif

  //Initialize the Window and Context
  Uint32 windowflags = SDL_WINDOW_OPENGL;
  if(!windowed) windowflags = windowflags | SDL_WINDOW_HIDDEN;

  gWindow = SDL_CreateWindow(_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, windowflags);
  if( gWindow == NULL ){
    printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }
  SDL_SetWindowResizable(gWindow, SDL_TRUE);

  gContext = SDL_GL_CreateContext(gWindow);
  if( gContext == NULL ){
    printf( "Context could not be created! SDL_Error: %s\n", SDL_GetError() );
    return false;
  }

  SDL_GL_SetSwapInterval(vsync);
  glewExperimental = GL_TRUE;     //Launch GLEW
  glewInit();

  #ifndef TINYENGINE_COMPATIBILITY
  if(antialias)
  glEnable(GL_MULTISAMPLE);
  #endif

  glEnable(GL_DEPTH_TEST);        //Setup Global OpenGL State!
  glDepthFunc(GL_LEQUAL);

  if(blend){
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  glEnable(GL_CULL_FACE);
  if(ccw) glFrontFace(GL_CCW);
  else glFrontFace(GL_CW);
  glLineWidth(lineWidth);
  glPointSize(pointSize);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
  printf("GL_VERSION: %s\n", glGetString(GL_VERSION));

  return true;
}

void View::quit(){
  SDL_GL_DeleteContext( gContext );
  SDL_DestroyWindow( gWindow );
}

void View::render(){
  (pipeline)();               //User-Defined Pipeline call
  SDL_GL_SwapWindow(gWindow); //Update Window
}

void View::target(glm::vec3 clearcolor, bool clearc, bool cleard){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(clearcolor.x, clearcolor.y, clearcolor.z, 1.0f);
  if(clearc) glClear(GL_COLOR_BUFFER_BIT);
  if(cleard) glClear(GL_DEPTH_BUFFER_BIT);
}

}
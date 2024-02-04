#ifndef TINYENGINE_VIEW
#define TINYENGINE_VIEW

#include <GL/glew.h>                                //Rendering Dependencies
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>

#include <TinyEngine/imgui>                    //Interface Dependencies
#include <TinyEngine/imgui_impl_sdl>
#include <TinyEngine/imgui_impl_opengl3>

#include <string>
#include <functional>

namespace Tiny {

struct View {

  bool init(std::string windowName, int width, int height);
  void quit();

  unsigned int WIDTH, HEIGHT;

  ImGuiIO io;
  std::function<void()> interface = [](){};  //User defined Interface
  bool showInterface = false;
  void drawInterface();

  std::function<void()> pipeline = [](){};           //User defined Pipeline
  void render();
  void target(glm::vec3 clearcolor, bool clearc = true, bool cleard = true);  //Target main window for drawing

  bool vsync = true;
  bool ccw = true;
  bool blend = true;
  unsigned int antialias = 16;
  float lineWidth = 1.0f;
  float pointSize = 1.0f;

  bool windowed = false;
  bool enabled = false;

  void toggle_fullscreen() {
    _fullscreen = !_fullscreen;
    if(!_fullscreen) SDL_SetWindowFullscreen(this->gWindow, 0);
    else SDL_SetWindowFullscreen(this->gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
  }

private:
  SDL_Window* gWindow;        //Window Pointer
  SDL_GLContext gContext;     //Render Context

  bool _fullscreen = false;
};

}

#endif

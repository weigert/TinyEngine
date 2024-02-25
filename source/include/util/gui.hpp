#ifndef TINYENGINE_UTIL_GUI
#define TINYENGINE_UTIL_GUI

#include <TinyEngine/TinyEngine>

#include <TinyEngine/imgui>                    //Interface Dependencies
#include <TinyEngine/imgui_impl_sdl>
#include <TinyEngine/imgui_impl_opengl3>

#include <functional>

namespace Tiny {

//! GUI is a simple lambda-based GUI wrapper, implemented w. DearImgui
//!
//! A GUI is constructed with a lambda, which calls Imgui methods.
//! A GUI can be directly rendered in the regular pipeline callback.
//! Hooking a GUI is necessary for interaction and visibility toggling.
struct GUI {
private:
  GUI(){
    IMGUI_CHECKVERSION();           //Setup ImGUI
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    ImGui_ImplSDL2_InitForOpenGL(Tiny::view.gWindow, Tiny::view.gContext);
    #ifndef TINYENGINE_COMPATIBILITY
    ImGui_ImplOpenGL3_Init("#version 330 core");
    #else
    ImGui_ImplOpenGL3_Init("#version 130");
    #endif
    ImGui::StyleColorsCustom();
    this->gWindow = Tiny::view.gWindow;
  }

public:
  template<typename F>
  GUI(F&& pipeline):GUI(){
    this->pipeline = pipeline;
  }

  ~GUI(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
  }

  // Event-Processing
  void hook(){
    Tiny::event.raw([](SDL_Event* in){
      ImGui_ImplSDL2_ProcessEvent(in);
    });
    Tiny::event.press[SDLK_ESCAPE]([this](bool press){
      if(!press) visible = !visible;
    });
  }

  void render(){

    if(!visible)
      return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(gWindow);
    ImGui::NewFrame();

    (pipeline)();  //Draw user-defined interface
    //ImGui::ShowDemoWindow();  //Demo-Window (if you want)

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  }

private:
  ImGuiIO io;
  SDL_Window* gWindow;
  bool visible = false;
  std::function<void()> pipeline;
};

} // end of namespace Tiny

#endif
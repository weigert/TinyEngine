#ifndef TINYENGINE_UTIL_DEBUG
#define TINYENGINE_UTIL_DEBUG

#include <TinyEngine/gui>

namespace Tiny {

//! Debug is a GUI for rendering TinyEngine Self-Inspection Data
//!
//! The purpose of Debug is to merely be an observer of other state,
//! and to output it to a singular debug window which can be universally
//! accessed. This includes logging data, but also static self-inspection.
//!
//! What has to happen is that the startup procedure is what hooks stuff in,
//! so tha the initialization order works correctly.
struct Debug: Tiny::GUI {

  using Tiny::GUI::GUI;

  Debug():Tiny::GUI([this](){
    this->interface();
  }){
    this->hook();
  }

private:

  //! 
  void hook(){
    Tiny::event.press[SDLK_F1]([this](bool press){
      if(!press) this->visible = !this->visible;
    });
  }

  void interface(){
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(Tiny::view.WIDTH, Tiny::view.HEIGHT), ImGuiCond_Always);
    // ... add debug info ...

    static bool p_open;

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;

    if(ImGui::Begin("TinyEngine 1.8 Debug Info", &p_open, window_flags)){
      ImGui::Text("TinyEngine 1.8 Debug Info");
      //... output debug info ...
      ImGui::End();
    }
  }
};

static Debug debug;

}

#endif
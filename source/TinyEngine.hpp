#ifndef TINYENGINE
#define TINYENGINE

#define TINYENGINE_VERSION "1.8"

// TinyEngine 1.8
// by Nicholas McDonald
// https://github.com/weigert/TinyEngine

#include <TinyEngine/Buffer>
#include <TinyEngine/Instance>
#include <TinyEngine/Model>
#include <TinyEngine/Shader>
#include <TinyEngine/Target>
#include <TinyEngine/Texture>

#include <TinyEngine/Event>
#include <TinyEngine/View>

//! TinyEngine's Main Namespace
namespace Tiny {

extern bool kill;   //!< Global Engine Shutdown Flag
extern View view;   //!< Global Engine View Manager
extern Event event; //!< Global Engine Event Handler

bool init();  //!< Initialize TinyEngine
void quit();  //!< Quit Gracefully

//! Initialize TinyEngine w.Window
bool window(std::string windowName, int width, int height);

//! Generic Templated Loop Callback
template<typename F, typename... Args>
void loop(F function, Args&&... args){
  while(!Tiny::kill){       // Engine-Active Check

    if(Tiny::view.enabled)  // Process Event Callbacks
      event.process();

    function(args...);      // Process User-Defined Loop

    if(Tiny::view.enabled)  // Process Rendering Pipeline
      view.render();

  }
}

} // end of namespace Tiny

#endif
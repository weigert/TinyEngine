# TinyEngine
Small OpenGL3 based 3D Engine / Wrapper in C++ with Networking

![Rendering Example Program](banner.png)

	LINES OF CODE (without unreasonable compression):
		Main File: 150
		Main Classes: 383
		Utility Classes: 544
		Network Classes: 104
		Helpers Namespaces: 486
		Total: 1667

	History:
		12. April 2020: 885
		29. April 2020: 1116
		17. May 2020: 1667

## Description
Based on many previous OpenGL3 projects, I have a very good idea of what features I need in an engine to build visually appealing 3D visualizations of generated data. Many of the projects had the same overarching structure, and used the same OpenGL wrapping structures. This engine unifies those basic concepts.

The goal of this "engine" is to act as an intuitive OpenGL wrapper for features which I use regularly, allowing me to develop 3D visualizations of generated data quickly and easily. I also want to keep it as small as possible, giving only necessary abstractions of boilerplate OpenGL code, and doing the rest with user-defined behaviors passed to the engine.

This is also a learning project, for practicing engine / systems design. This is not intended as an optimized game development library, but can be used to generate beatiful 3D visualizations of generated data.

If anybody likes the structure, they are free to adopt it or recommended additions / changes.

## Examples
![Multi-Julia Animation](julia.gif)

Animated Julia-Set (Example 4). See my blog [here](https://weigert.vsos.ethz.ch/2020/04/14/animated-multi-julia-sets/).

## Structure
The engine consists of four main components:
  - View Class: 	Window handling and rendering.
  - Event Class: 	Event handling for keyboard and mouse.
  - Audio Class: 	Audio interface for playing / looping sounds.
  - Net Class:		Networking interface for sending and receiving data packages

A number of utility classes wrap typical OpenGL features into easily useable structures. These have simple constructors and destructors that wrap the necessary OpenGL so you don't have to worry about it:

  - Texture: Depth and / or color components. Construct as empty, from raw surface data or raw data using an algorithm.
  - Shader: Load, compile, link and use shader programs easily (vertex, fragment, geometry)
  - Model: Model data wrapper. Construct using user-defined function. Handles data loading / updating, rendering, movement.
  - Target: Lets you bind a texture for render targeting (FBO wrapper). 2D (billboards) and 3D (cubemaps).
  - Particle: Instanced rendering wrapper. Lets you push a bunch of model matrices and then instance render any model.
 
The behavior is combined through a standard game pipeline. Applications can define functions that are executed in certain parts of the pipeline. These include:

  - User defined event-handler: Can read inputs from the event class and execute arbitrary code.
  - User defined render-pipeline: Combines all of the utility classes to draw to the window, however you like.
  - User defined graphical interface: Can create an ImGUI window and read from / act on your data structures.
  - User defined game loop: Executed every cycle. Arbitrary code acting on your data structures.
  - User defined network event-handlers: Based on messages received via the network interface, execute code

A number of helper namespaces then supply additional algorithms and functions that are useful. More information on these can be found in the wiki: [Helper Namespaces](https://github.com/weigert/TinyEngine/wiki/Helper-Namespaces).

## Dependencies

    - OpenGL3:
    - SDL2 (Core, Image, TTF, Mixer):
    - GLEW:
    - Boost (System, Filesystem):
  
    - DearImGUI (Already included!)
    - gcc (for compilation)
  
Tested on Ubuntu 18 LTS.

## Usage
Include the main file `TinyEngine.h` and construct the problem as described below.

    #include "TinyEngine.h"
    int main( int argc, char* args[] ) {
      //...
      return 0;
    }

### Constructing a Program
Open a window using:

    //...
    Tiny::window("Example Window", 500, 500);

Add an event handler:

    //Event Handler
    Tiny::event.handler = [&](){ /* ... triggers are in Tiny::event ... */ 
    	if(Tiny::event.press["SDLK_a"])
		std::cout<<"A is being pressed!"<<std::endl;
		
	//...
    };

Define a user interface (by default visibility is toggled with ESC):

    //Graphical User Interface
    Tiny::view.interface = [&](){ /* ... your IMGUI code here... */ 
    	  ImGui::Begin("Example Interface", NULL, ImGuiWindowFlags_None);
	  
	  //...
	  
	  ImGui::End();
    };

Define a rendering pipeline (after defining your utility classes to be used in the pipeline):

    //Rendering Pipeline
    Tiny::view.pipeline = [&](){ /* ... */ };

Execute the full game loop with any additional code:

    //Execute the render loop
    Tiny::loop([&](){
		  //Your additional code here...
    });
    
Close the program using:

    Tiny::quit();
    //...
    
All of these elements can be defined directly using lambdas, or set using functionals. Just make sure context is always available, and it will work. Access the audio interface using `Tiny::audio`. Any additional parameters in the view or event class should be set before opening the window.

See the example programs (they are very brief) to see how a typical program is constructed, and what utility classes are applied.

### Utility Classes
Information on how to use the utility classes is given in the Wiki:

[Utility Classes](https://github.com/weigert/TinyEngine/wiki/Utility-Classes)

### Rendering Pipeline
The rendering pipeline can be constructed in any way, but generally consists of the following steps:
 
  - Choose rendering target (Tiny.view, billboard)
  - Setup shader (use, set uniforms)
  - Render models (sprites and models, possibly billboards)
  - Repeat!
      
### Compiling and Linking
See the example programs to see exactly how to link the program (makefile). Compiled using gcc on Ubuntu 18 LTS.

## To-Do
	- Audio Interface isn't very good because I haven't built many audio interfaces in the past, just basic ones. I will have to do a project focused around playing sounds to improve it adequately.
	- Event handler is capable of doing good things, but is hard to get right intuitively because many inputs are passed very quickly. I need to rethink this a little.
	
## License
MIT License

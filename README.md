# TinyEngine
Small OpenGL3 based 3D Engine / Wrapper in C++

![Rendering Example Program](banner.png)

	LINES OF CODE (without unreasonable compression):
		Main File: 106
		Main Classes: 258
		Utility Classes: 546
		Helpers Namespaces: 206
		Total: 1116

	History:
		12. April 2020: 885
		29. April 2020: 1116

## Description
Based on many previous OpenGL3 projects, I have a very good idea of what features I need in an engine to build visually appealing 3D visualizations of generated data. Many of the projects had the same overarching structure, and used the same OpenGL wrapping structures. This engine unifies those basic concepts.

The goal of this "engine" is to act as an intuitive OpenGL wrapper for features which I use regularly, allowing me to develop 3D visualizations of generated data quickly and easily. I also want to keep it as small as possible, giving only necessary abstractions of boilerplate OpenGL code, and doing the rest with user-defined behaviors passed to the engine.

This is also a learning project, for practicing engine / systems design. This is not intended as an optimized game development library, but can be used to generate beatiful 3D visualizations of generated data.

If anybody likes the structure, they are free to adopt it or recommended additions / changes.

## Examples
![Multi-Julia Animation](julia.gif)

Animated Julia-Set (Example 4). See my blog [here](https://weigert.vsos.ethz.ch/2020/04/14/animated-multi-julia-sets/).

## Structure
The engine consists of three main components:
  - View Class: Window handling and rendering.
  - Event Class: Event handling for keyboard and mouse.
  - Audio Class: Audio interface for playing / looping sounds.

A number of utility classes allow for simple integration of typical OpenGL behavior:
  - Shader: Load, compile, link and use shader programs easily.
  - Model: 3D model wrapper. Construct using user-defined function. Handles updating, rendering and movement.
  - Sprite: 2D texture wrapper. Handles loading from images, updating, rendering.
  - Billboard: FBO render target wrapper, easy targeting for drawing and rendering.
 
The behavior is combined through a standard game pipeline. Applications can define functions that are executed in certain parts of the pipeline. These include:
  - User defined event-handler: Can read inputs from the event class and execute arbitrary code.
  - User defined render-pipeline: Combines all of the utility classes to draw to the window, however you like.
  - User defined graphical interface: Can create an ImGUI window and read from / act on your data structures.
  - User defined game loop: Executed every cycle. Arbitrary code acting on your data structures.

A number of helper namespaces then supply additional algorithms and functions that are useful:
  - timer: Benchmarking and code execution on a timed cycle in a separate thread.
  - image: Loading / saving images from files, constructing textures from raw data using user-defined methods.
  - draw: Pre-made methods for generating textures from raw data.
  - color: Color helper, with bezier color interpolation from color schemes. Color hashing.

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
    Tiny::init("Example Window", 500, 500);
 
Add an event handler:

    //Event Handler
    Tiny::event.handler = [&](){ /* ... */ };

Define a user interface (by default visibility is toggled with ESC):

    //Graphical User Interface
    Tiny::view.interface = [&](){ /* ... */ };

Define a rendering pipeline:

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

### Utility Classes
Utility classes wrap boilerplate OpenGL behavior. The classes are extremely small, so it is worth reading through their code to understand what they do and how they can be modified for your specific needs. Otherwise, their default constructors are usually sufficient! These classes have a destructor included that deletes all the data inside, so you don't have to.

Shader:

      //Construction
      Shader shader("vertexshader.vs", "fragmentshader.fs", {"all", "shader", "input", "properties"});
      
      //Activation
      shader.use();
      
      //Uniform Setting (fully templated - automatic handling of type)
      shader.uniform(string name, int value);
      shader.uniform(string name, glm::vec3 vec);
      shader.uniform(string name, glm::mat4 mat);
      //... etc
      
Sprite:

      //Construction
      Sprite sprite(SDL_Surface* s);
      Sprite sprite(image::load("filename.png"));         //From image
      Sprite sprite(image::make(size, data, algorithm));  //From algorithm
      
      //Updating
      sprite.update(SDL_Surface* s);
      
      //Rendering
      sprite.render();
      
Model:

      //Construction
      Model model(algorithm); //User-defined construction algorithm
      
      //Reconstruct model
      model.construct(algorithm);
      
      //Update buffer data
      model.update();
      
      //Rendering
      model.render();
      
Algorithm is passed as a functional or lambda, that needs to capture the data that the model will be constructed from.

Billboard:

      //Construction
      Billboard billboard(int width, int height, bool depthOnly);
      
      //Targeting
      billboard.target(vec3 clearcolor); //This billboard ('s FBO) is now the render target
      
      //Rendering
      billboard.render();
      
To target the main window again, call:

      //
      Tiny::view.target(vec3 clearcolor);
      
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

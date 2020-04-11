# TinyEngine
Small OpenGL3 based 3D Engine / Wrapper in C++

## Description
This is the full maintained version of my OpenGL3 based 3D engine as I add features to it. 
I use this or a stripped-down variant for a lot of my projects.

This tiny engine was made for learning OpenGL3 and has served me very well in my projects since. Feel free to use it / adapt it / suggest features.
The simplicity of the wrapper makes it easy to hack, modify on the low level and study.

This is by no means "optimized"!

## Features
Model Class:
  - Wrapper for constructing and rendering VAO / VBO data.
  - Templated model generation, so your arbitrary data structure can be converted to a 3D structure and drawn.
  
Shaders:
  - Wrapper for loading / compiling / linking and using GLSL shaders from files.
  - Small shader library, for rendering billboards, shadow mapping, phong shading
    
Billboards:
    - FBO rendering support, allows for pipelining and utilization of effect shaders
    - Drawing of textures to the screen
    - Easy positioning of the billboard on the screen
    - Drawing helper for generating a texture from arbitrary (e.g. array) data, using a set of drawing helper
    
Interface:
    - Integrates DearImGUI in a way to give you an easy way to provide an interface
    - 
    
Event Handler:
    - Support for keyboard and mouse inputs
    - 
    
Audio:
    - Capable of playing / looping audio files using event triggers

## Dependencies
... coming soon ...

## Usage
... coming soon ...

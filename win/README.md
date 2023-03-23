
# TinyEngine - Windows Port
Tiny OpenGL3 based 2D/3D Engine / Wrapper in C++
This port will allow you to compile TinyEngine and its examples on Windows. 

## About
The original TinyEngine source remains unchanged for the Windows port as it was already written in a portable manner. The exception to this is TinyEngine.h which has 3 #include's added to fix compile errors. The original TinyEngine.h is untouched and the modified copy is in the "win/TinyEngineWin" directory.  

All 11 example projects (as of 01/12/2020) have been included in one Visual Studio solution file, with each example in its own project. An extra template project (x_Name Template) is included and can easily be duplicated and renamed to create a new project with the minimum number of files needed to compile. In most instances the example code was copied verbatim, with only changes to relative include paths necessary. 6_Tree required some extra work to fix a vector subscript out of range error, but only differs by around 10 lines. 

All examples were built with Visual Studio Community 2019, version 16.8.2, against the C++ 17 compiler. 

## Preparing your environment

As I have been away from C++ for a while I thought now would be a good time to try vcpkg. I was impressed by how easy it was to use and recommend utilising it to set up your environment. The following steps will walk you through the process of installing vcpkg and the necessary dependencies. 

### Installing vcpkg
Follow the instructions at https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=msvc-160#installation to clone the vcpkg repository and bootstrap it. 

Make sure to follow the integration instructions at https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=msvc-160#installation on a per-user basis so that include paths are resolved automagically. 

### Installing dependencies
The following packages need to be installed to compile TinyEngine and the examples. This can be done from a Developer Power Shell in Visual Studio. Make sure you change to the directory where the vcpkg binary resides. 

#### SDL2:
	./vcpkg install sdl2:x86-windows
	./vcpkg install sdl2-image:x86-windows
	./vcpkg install sdl2-ttf:x86-windows
	./vcpkg install sdl2-mixer:x86-windows

#### GLEW 
	./vcpkg install glew:x86-windows

#### GLM 
	./vcpkg install glm:x86-windows

#### Boost Filesystem
	./vcpkg install boost-filesystem:x86-windows

#### Libnoise
	./vcpkg install libnoise:x86-windows

### SDL2 path environment variable
The Visual Studio project makes use of an environment variable named SDL2_DIR. You'll either need to set this environment variable or the projects will fail to link. SDL2 was installed into the vcpkg directory in a previous step, the path will be something similar to

	C:\vcpkg\packages\sdl2_x86-windows

## Trying out the examples
Open the TinyEngineWin.sln solution file in the "win/TinyEngineWin" directory, set whichever example you want to launch as your startup project, then build and run. If you have any issues please open an issue and I'll do my best to help you resolve it. 
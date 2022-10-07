# TinyEngine Makefile Example
# Compiler Configuration

CC = g++ -std=c++17
CF = -Wfatal-errors -O
LF = -I/usr/local/include -L/usr/local/lib

# Linking Configuration

TINYLINK = -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGLEW -lboost_system -lboost_filesystem

FILES = 000_Empty 001_Windowless 01_Image 02_Heightmap 03_Automata 04_Julia 05_Particles 06_Tree 07_SDF 08_Raymarch 09_Scene \
10_Audio 11_Voronoi 12_Diffusion 13_Dither 15_Compute 16_Gravity 17_ODE3D

# OS Specific Linking

UNAME := $(shell uname)

## Detect GNU/Linux
ifeq ($(UNAME), Linux)			
TINYLINK += -lX11 -lGL
endif

## Detect MacOS
ifeq ($(UNAME), Darwin)			
TINYLINK += -framework OpenGL -DTINYENGINE_OS_MAC
LF += -I/opt/homebrew/include -L/opt/homebrew/lib
CC = g++-12 -std=c++17

FILES = 000_Empty 001_Windowless 01_Image 02_Heightmap 03_Automata 04_Julia 05_Particles 06_Tree 07_SDF 08_Raymarch 09_Scene \
10_Audio 11_Voronoi 12_Diffusion 13_Dither 14_Renderpool 16_Gravity 17_ODE3D
endif

clean:
	$(foreach i,$(FILES), rm $(i)/main;)

all:
	$(foreach i,$(FILES), $(CC) $(i)/main.cpp $(CF) $(LF) -lTinyEngine $(TINYOS) $(TINYLINK) -o $(i)/main;)

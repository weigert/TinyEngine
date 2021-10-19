# TinyEngine Makefile Example
# Compiler Configuration

CC = g++ -std=c++17
CF = -Wfatal-errors -O
LF = -I/usr/local/include -L/usr/local/lib

# Linking Configuration

TINYLINK = -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGLEW -lboost_system -lboost_filesystem

# Operating System Specific Linking

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)			#Detect GNU/Linux
TINYOS = -lX11 -lGL
endif
ifeq ($(UNAME), Darwin)			#Detext MacOS
TINYOS = -framework OpenGL
endif

FILES = 0_Empty 01_Windowless 1_Image 2_Heightmap 3_Automata 4_Julia 5_Particles 6_Tree 7_SDF 8_Raymarch 9_Scene \
10_Audio 11_Voronoi 12_Diffusion 13_Dither 15_Compute 16_Gravity 17_ODE3D

all:
			$(foreach i,$(FILES), $(CC) $(i)/main.cpp $(CF) $(LF) -lTinyEngine $(TINYOS) $(TINYLINK) -o $(i)/main;)

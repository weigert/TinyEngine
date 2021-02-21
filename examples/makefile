CC = g++ -std=c++17
CF = -Wfatal-errors -O
LF = -I/usr/local/include -L/usr/local/lib

TINYLINK = -lX11 -lpthread -lSDL2 -lnoise -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lGLEW -lboost_serialization -lboost_system -lboost_filesystem

FILES = 0_Empty 1_Image 2_Heightmap 3_Automata 4_Julia 5_Particles 6_Tree 7_SDF 8_Raymarch 9_Scene \
10_Audio 11_Voronoi 12_Diffusion 13_Dither

all:
			$(foreach i,$(FILES), $(CC) $(i)/main.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o $(i)/main;)

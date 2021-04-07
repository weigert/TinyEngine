CC = g++-10 -std=c++20 -ggdb3
CF = -Wfatal-errors -O3
LF = -I/usr/local/include -L/usr/local/lib

TINYLINK = -lX11 -lpthread -lSDL2 -lnoise -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lGLEW -lboost_serialization -lboost_system -lboost_filesystem

all: fast other

fast: source/main.cpp
			$(CC) source/main.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o 0main

other: source/naive.cpp
			$(CC) source/naive.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o 1naive

CC = g++-10 -std=c++20
CF = -Wfatal-errors -O3
LF = -I/usr/local/include -L/usr/local/lib

TINYLINK = -lX11 -lpthread -lSDL2 -lnoise -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lGLEW -lboost_serialization -lboost_system -lboost_filesystem

all: main.cpp
			$(CC) main.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o main

other: naive.cpp
			$(CC) naive.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o naive

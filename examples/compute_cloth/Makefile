# TinyEngine Makefile
# Compiler Configuration

CC = g++ -std=c++17
CF = -Wfatal-errors -O
LF = -I$(HOME)/.local/include -L$(HOME)/.local/lib

# General Linking

TINYLINK = -lpthread -lSDL2 -lpng -lSDL2_mixer -lSDL2_ttf -lGLEW 

# OS Specific Linking

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)			#Detect GNU/Linux
TINYOS = -lX11 -lGL
endif
ifeq ($(UNAME), Darwin)			#Detext MacOS
TINYOS = -framework OpenGL
endif

all: main.cpp
	@$(CC) main.cpp $(CF) $(LF) -lTinyEngine $(TINYOS) $(TINYLINK) -o main

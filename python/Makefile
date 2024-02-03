# TinyEngine/python Makefile
# Compiler Configuration

CC = g++ -std=c++17
CF = -Wfatal-errors -O
LF = -I$(HOME)/.local/include -L$(HOME)/.local/lib

# General Linking

TINYLINK = -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGLEW 

# OS Specific Linking

UNAME := $(shell uname)

## Detect GNU/LinuxF
ifeq ($(UNAME), Linux)
TINYLINK += -lX11 -lGL
endif

## Detect MacOS
ifeq ($(UNAME), Darwin)
TINYLINK += -framework OpenGL -DTINYENGINE_OS_MAC
LF += -I/opt/homebrew/include -L/opt/homebrew/lib
CC = g++-12 -std=c++17
endif

all:
	@$(eval MODULE:= tinyengine$(shell python3-config --extension-suffix))
	@$(CC) source/TinyEngine.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -shared -fPIC $(shell python3 -m pybind11 --includes) -o ${MODULE}
	@echo "Installing TinyEngine Python Module..."
	@mv ${MODULE} $(HOME)/.local/lib/python3.12/site-packages/${MODULE}
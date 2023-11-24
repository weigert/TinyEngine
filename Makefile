# TinyEngine Build Recipe
# Author: Nicholas McDonald
# Version 1.7
# Tested on GNU/Linux

# Install Location Configuration
LIBPATH = $(HOME)/.local/lib
INCPATH = $(HOME)/.local/include

# Compilation Settings
CC = g++ -std=c++17
CF = -Wfatal-errors -O3

# MacOS: Dependencies install with homegrew

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)			#Detect GNU/Linux
TINYOS = -lX11 -lGL
endif
ifeq ($(UNAME), Darwin)			#Detext MacOS
INCPATH = /opt/homebrew/include
CC = g++-12 -std=c++17
TINYOS = -framework OpenGL
endif

##########################
#  TinyEngine Installer  #
##########################

all: setup helpers install

# Copy Required Header Files to Target Location
.PHONY: setup
setup:
			@echo "Copying Core Header Files ...";
			@if [ ! -d $(INCPATH) ]; then mkdir $(INCPATH); fi;
			@if [ ! -d $(INCPATH)/TinyEngine ]; then mkdir $(INCPATH)/TinyEngine; fi;
			@cp TinyEngine.hpp $(INCPATH)/TinyEngine/TinyEngine
			@cp include/audio.hpp $(INCPATH)/TinyEngine/Audio
			@cp include/event.hpp $(INCPATH)/TinyEngine/Event
			@cp include/view.hpp $(INCPATH)/TinyEngine/View
			@cp include/utility/buffer.hpp $(INCPATH)/TinyEngine/Buffer
			@cp include/utility/instance.hpp $(INCPATH)/TinyEngine/Instance
			@cp include/utility/model.hpp $(INCPATH)/TinyEngine/Model
			@cp include/utility/shader.hpp $(INCPATH)/TinyEngine/Shader
			@cp include/utility/texture.hpp $(INCPATH)/TinyEngine/Texture
			@cp include/utility/target.hpp $(INCPATH)/TinyEngine/Target
			@cp include/imgui/imgui.h $(INCPATH)/TinyEngine/imgui
			@cp include/imgui/imgui_impl_opengl3.h $(INCPATH)/TinyEngine/imgui_impl_opengl3
			@cp include/imgui/imgui_impl_sdl.h $(INCPATH)/TinyEngine/imgui_impl_sdl
			@cp include/imgui/imconfig.h $(INCPATH)/TinyEngine/imconfig.h
			@echo "Done";

#######################
# TinyEngine Compiler #
#######################

IMGUI = imgui imgui_demo imgui_draw imgui_widgets imgui_impl_opengl3 imgui_impl_sdl

.PHONY: install
install:
			@echo "Compiling TinyEngine ...";
			@if [ ! -d "tmp" ]; then mkdir tmp; fi;
			@$(CC) $(CF) -I$(INCPATH) $(TINYOS) -c -fPIC TinyEngine.cpp -o tmp/TinyEngine.o
			@$(foreach i,$(IMGUI), $(CC) $(CF) -I$(INCPATH) $(TINYOS) -c -fPIC include/imgui/$(i).cpp -o tmp/$(i).o;)
			@echo "Generating Static Library Archive ...";
			@ar cr tmp/libTinyEngine.a tmp/TinyEngine.o tmp/imgui.o tmp/imgui_demo.o tmp/imgui_draw.o tmp/imgui_widgets.o tmp/imgui_impl_opengl3.o tmp/imgui_impl_sdl.o
			@echo "Placing Compiled TinyEngine Library ...";
			@if [ ! -d $(LIBPATH) ]; then mkdir $(LIBPATH); fi;
			@cp tmp/libTinyEngine.a $(LIBPATH)/libTinyEngine.a
			@rm -rf tmp
			@echo "Done";

######################
# TinyEngine Helpers #
######################

HELPERS = camera color math image object timer parse log

.PHONY: helpers
helpers:
			@echo "Copying Helper Header Files ...";
			@if [ ! -d "$(INCPATH)" ]; then mkdir $(INCPATH); fi;
			@if [ ! -d "$(INCPATH)/TinyEngine" ]; then mkdir $(INCPATH)/TinyEngine; fi;
			@$(foreach var,$(HELPERS), cp include/helpers/$(var).hpp $(INCPATH)/TinyEngine/$(var);)
			@echo "Done";

.PHONY: examples
examples:
	@cd examples; $(MAKE) all
# TinyEngine Build Recipe
# Author: Nicholas McDonald
# Version 1.0
# Tested on GNU/Linux

# Install Location Configuration
LIBPATH = /usr/local/lib
INCLUDEPATH = /usr/local/include

# Compilation Settings
CC = g++ -std=c++17
CF = -Wfatal-errors -O3

##########################
#  TinyEngine Installer  #
##########################

all: setup helpers install

# Copy Required Header Files to Target Location
setup:
			@echo "Copying Core Header Files ...";
			@if [ ! -d "/usr/local/include/TinyEngine" ]; then mkdir $(INCLUDEPATH)/TinyEngine; fi;
			@cp TinyEngine.h $(INCLUDEPATH)/TinyEngine/TinyEngine
			@cp include/audio.h $(INCLUDEPATH)/TinyEngine/Audio
			@cp include/event.h $(INCLUDEPATH)/TinyEngine/Event
			@cp include/view.h $(INCLUDEPATH)/TinyEngine/View
			@cp include/utility/instance.cpp $(INCLUDEPATH)/TinyEngine/Instance
			@cp include/utility/model.cpp $(INCLUDEPATH)/TinyEngine/Model
			@cp include/utility/shader.cpp $(INCLUDEPATH)/TinyEngine/Shader
			@cp include/utility/texture.cpp $(INCLUDEPATH)/TinyEngine/Texture
			@cp include/utility/target.cpp $(INCLUDEPATH)/TinyEngine/Target
			@cp include/imgui/imgui.h $(INCLUDEPATH)/TinyEngine/imgui
			@cp include/imgui/imgui_impl_opengl3.h $(INCLUDEPATH)/TinyEngine/imgui_impl_opengl3
			@cp include/imgui/imgui_impl_sdl.h $(INCLUDEPATH)/TinyEngine/imgui_impl_sdl
			@cp include/imgui/imconfig.h $(INCLUDEPATH)/TinyEngine/imconfig.h
			@echo "Done";

#######################
# TinyEngine Compiler #
#######################

IMGUI = imgui imgui_demo imgui_draw imgui_widgets imgui_impl_opengl3 imgui_impl_sdl

install:
			@echo "Compiling TinyEngine ...";
			@if [ ! -d "tmp" ]; then mkdir tmp; fi;
			@$(CC) $(CF) -c -fPIC TinyEngine.cpp -o tmp/TinyEngine.o
			@$(foreach i,$(IMGUI), $(CC) $(CF) -c -fPIC include/imgui/$(i).cpp -o tmp/$(i).o;)
			@echo "Generating Static Library Archive ...";
			@ar cr tmp/libTinyEngine.a tmp/TinyEngine.o tmp/imgui.o tmp/imgui_demo.o tmp/imgui_draw.o tmp/imgui_widgets.o tmp/imgui_impl_opengl3.o tmp/imgui_impl_sdl.o
			@echo "Placing Compiled TinyEngine Library ...";
			@cp tmp/libTinyEngine.a $(LIBPATH)/libTinyEngine.a
			@rm -rf tmp
			@echo "Done";

######################
# TinyEngine Helpers #
######################

HELPERS = camera color helper image object

helpers:
			@echo "Copying Helper Header Files ...";
			@if [ ! -d "/usr/local/include/TinyEngine" ]; then mkdir $(INCLUDEPATH)/TinyEngine; fi;
			@$(foreach var,$(HELPERS), cp include/helpers/$(var).h $(INCLUDEPATH)/TinyEngine/$(var);)
			@echo "Done";

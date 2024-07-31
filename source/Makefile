# TinyEngine/source
# Make Ruless

# Install Location Configuration
LIBPATH = $(HOME)/.local/lib
INCPATH = $(HOME)/.local/include

# Compilation Settings
CC = g++ -std=c++17 $(CPPFLAGS)
CF = -Wfatal-errors -O3

# MacOS: Dependencies install with homegrew

UNAME := $(shell uname)
ifeq ($(UNAME), Linux)			#Detect GNU/Linux
TINYOS = -lX11 -lGL
endif
ifeq ($(UNAME), Darwin)			#Detext MacOS
INCPATH = /opt/homebrew/include
CC = g++-12 -std=c++17 $(CPPFLAGS)
TINYOS = -framework OpenGL
endif

##########################
#  TinyEngine Installer  #
##########################

all: setup helpers install

# Copy Required Header Files to Target Location
.PHONY: setup
setup:
	@echo "TinyEngine: Copying Core Header Files ...";
	@if [ ! -d $(INCPATH) ]; then mkdir $(INCPATH); fi;
	@if [ ! -d $(INCPATH)/TinyEngine ]; then mkdir $(INCPATH)/TinyEngine; fi;
	@cp TinyEngine.hpp $(INCPATH)/TinyEngine/TinyEngine
	@cp include/event.hpp $(INCPATH)/TinyEngine/Event
	@cp include/view.hpp $(INCPATH)/TinyEngine/View
	@cp include/core/buffer.hpp $(INCPATH)/TinyEngine/Buffer
	@cp include/core/instance.hpp $(INCPATH)/TinyEngine/Instance
	@cp include/core/model.hpp $(INCPATH)/TinyEngine/Model
	@cp include/core/shader.hpp $(INCPATH)/TinyEngine/Shader
	@cp include/core/texture.hpp $(INCPATH)/TinyEngine/Texture
	@cp include/core/target.hpp $(INCPATH)/TinyEngine/Target
	@cp include/external/imgui/imgui.h $(INCPATH)/TinyEngine/imgui
	@cp include/external/imgui/imgui_impl_opengl3.h $(INCPATH)/TinyEngine/imgui_impl_opengl3
	@cp include/external/imgui/imgui_impl_sdl.h $(INCPATH)/TinyEngine/imgui_impl_sdl
	@cp include/external/imgui/imconfig.h $(INCPATH)/TinyEngine/imconfig.h

#######################
# TinyEngine Compiler #
#######################

IMGUI = imgui imgui_demo imgui_draw imgui_widgets imgui_impl_opengl3 imgui_impl_sdl

.PHONY: install
install:
	@echo "TinyEngine: Compiling TinyEngine ...";
	@if [ ! -d "tmp" ]; then mkdir tmp; fi;
	@$(CC) $(CF) -I$(INCPATH) $(TINYOS) -c -fPIC TinyEngine.cpp -o tmp/TinyEngine.o
	@$(CC) $(CF) -I$(INCPATH) $(TINYOS) -c -fPIC include/event.cpp -o tmp/event.o
	@$(CC) $(CF) -I$(INCPATH) $(TINYOS) -c -fPIC include/view.cpp -o tmp/view.o
	@$(foreach i,$(IMGUI), $(CC) $(CF) -I$(INCPATH) $(TINYOS) -c -fPIC include/external/imgui/$(i).cpp -o tmp/$(i).o;)
	@echo "TinyEngine: Generating Static Library Archive ...";
	@ar cr tmp/libTinyEngine.a tmp/TinyEngine.o tmp/event.o tmp/view.o tmp/imgui.o tmp/imgui_demo.o tmp/imgui_draw.o tmp/imgui_widgets.o tmp/imgui_impl_opengl3.o tmp/imgui_impl_sdl.o
	@echo "TinyEngine: Placing Compiled TinyEngine Library ...";
	@if [ ! -d $(LIBPATH) ]; then mkdir $(LIBPATH); fi;
	@cp tmp/libTinyEngine.a $(LIBPATH)/libTinyEngine.a
	@rm -rf tmp

######################
# TinyEngine Helpers #
######################

HELPERS = camera image object benchmark gui debug

.PHONY: helpers
helpers:
	@echo "TinyEngine: Copying Helper Header Files ...";
	@if [ ! -d "$(INCPATH)" ]; then mkdir $(INCPATH); fi;
	@if [ ! -d "$(INCPATH)/TinyEngine" ]; then mkdir $(INCPATH)/TinyEngine; fi;
	@$(foreach var,$(HELPERS), cp include/util/$(var).hpp $(INCPATH)/TinyEngine/$(var);)

TINY = ../../include/imgui/imgui.cpp ../../include/imgui/imgui_demo.cpp ../../include/imgui/imgui_draw.cpp ../../include/imgui/imgui_widgets.cpp ../../include/imgui/imgui_impl_opengl3.cpp ../../include/imgui/imgui_impl_sdl.cpp
TINYLINK = -lX11 -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGL -lGLEW -lboost_serialization -lboost_system -lboost_filesystem

CC = g++ -std=c++17
COMPILER_FLAGS = -Wfatal-errors -O
LINKER_FLAGS = -I/usr/local/include -L/usr/local/lib

.PHONY all: server client

server: 0_server.cpp
			$(CC) 0_server.cpp $(TINY) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(TINYLINK) -o server

client: 1_client.cpp
			$(CC) 1_client.cpp $(TINY) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(TINYLINK) -o client

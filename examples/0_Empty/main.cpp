#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	//Initialize a Window
	Tiny::window("Example Window", 600, 400);

	//Add the Event Handler
	Tiny::event.handler = [&](){
	};

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){
	};

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){
	};

	//Execute the render loop
	Tiny::loop([&](){
		//Do any additional stuff in here
	});

	Tiny::quit();

	return 0;
}

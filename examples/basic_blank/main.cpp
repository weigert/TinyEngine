#include <TinyEngine/TinyEngine>

int main( int argc, char* args[] ) {

	//Initialize a Window
	Tiny::window("Example Window", 600, 400);

	//Define the rendering pipeline
	Tiny::view.pipeline = [](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white

	};

	//Execute the render loop
	Tiny::loop([&](){
	});

	Tiny::quit();

	return 0;

}
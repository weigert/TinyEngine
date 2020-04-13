#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	//Add the Event Handler
	Tiny::event.handler 	= [&](){ /* ... */ };

	//Set up an Interface
	Tiny::view.interface 	= [&](){ /* ... */ };

	//Launch the Window
	Tiny::init("Image loading and Rendering with Shader", 1200, 800);

	//Load a Shader
	Shader billboardShader("billboard.vs", "billboard.fs", {"in_Quad", "in_Tex"});

	//Create billboard from image, position
	Billboard image("hunter.png");
	image.move(glm::vec2(0), glm::vec2(0.2));

	//Drawing Pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);			//Render Target

		billboardShader.use();								//Prepare Shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image.texture);
		billboardShader.setInt("imageTexture", 0);
		billboardShader.setMat4("model", image.model);

		image.render();												//Render Objects

	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */ });

	Tiny::quit();

	return 0;
}

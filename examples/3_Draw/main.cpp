#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	//Launch the Window
	Tiny::init("Heightmap Render", WIDTH, HEIGHT);

	//Add an Event Handler for camera movement
	Tiny::event.handler = eventHandler;

	//Set up an Interface
	Tiny::view.interface = [&](){ /* ... */

	};

	//Setup the Model Stuff
	setup();

	Sprite image;	//Create an image from the heightmap in black and white
	image.raw(image::make<double>(dim, heightmap, draw::grass));

	Shader billboardShader("shader/billboard.vs", "shader/billboard.fs", {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		//Prepare Shader
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		billboardShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image.texture);
		billboardShader.setInt("imageTexture", 0);
		billboardShader.setMat4("model", image.model);
		image.render();

	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

	});

	Tiny::quit();

	return 0;
}

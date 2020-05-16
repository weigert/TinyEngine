#include "../../TinyEngine.h"
#include "effects.h"

int main( int argc, char* args[] ) {

	Tiny::window("Shader Effects Example", 1200, 800);

	Tiny::event.handler = [](){ /* ... */ };

	Tiny::view.interface = interfaceFunc;

	//Create Render Target and load raw image data into the texture
	Target<GL_TEXTURE_2D> board(1200, 800, false);
	board.texture.raw<GL_TEXTURE_2D>(image::load("canyon.png"));

	//Create a flat model primitive for drawing the billboard to screen
	Primitive<SQUARE2D> flat;

	//Load our effect shader and set the inputs of the shader
	Shader effect({"shader/effect.vs", "shader/effect.fs"}, {"in_Quad", "in_Tex"});

	//Render Pipeline
	Tiny::view.pipeline = [&](){

		//Target the screen for drawing, clear color black
		Tiny::view.target(color::black);

		effect.use();	//Setup Shader and uniforms
		effect.uniform("index", ind);
		effect.uniform("res", res);
		effect.uniform("bits", bits);

		//Load the Texture
		effect.texture("imageTexture", board.texture.texture);

		//Render the shader using our flat geometry
		effect.uniform("model", flat.model);
		flat.render(); //Render Sprite

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

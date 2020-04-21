#include "../../TinyEngine.h"
#include "effects.h"

int main( int argc, char* args[] ) {

	Tiny::init("Shader Effects Example", 1200, 800);

	Tiny::event.handler = [](){ /* ... */ };

	Tiny::view.interface = interfaceFunc;

	//Load the Sprite
	Billboard sprite(1200, 800, false);
	sprite.raw(image::load("canyon.png"));

	//Shaders
	Shader effect("shader/effect.vs", "shader/effect.fs", {"in_Quad", "in_Tex"});

	//Render Pipeline
	Tiny::view.pipeline = [&](){

		//Render Billboard to Screen
		Tiny::view.target(color::black);

		effect.use();	//Setup Shader
		effect.setInt("index", ind);
		effect.setInt("res", res);
		effect.setInt("bits", bits);

		//Add the Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sprite.texture);
		effect.setInt("imageTexture", 0);
		effect.setMat4("model", sprite.model);

		sprite.render(); //Render Sprite

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

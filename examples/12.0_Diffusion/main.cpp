#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;															//Turn off VSYNC before opening window

	Tiny::window("GPU Accelerated Cellular Automata Example", WIDTH, HEIGHT);	//Open Window
	Tiny::event.handler = eventHandler;										//Define Event Handler
	Tiny::view.interface = [](){ /* ... */ }; 						//No Interface

	setup();	//Setup Model Data

	Texture tex();		//Load Texture with Image

/*
	//Construct a billboard, using a texture generated from the raw data
	Billboard targetA(image::make<bool>(glm::vec2(WIDTH, HEIGHT), data, [](bool t){
		if(t) return glm::vec4(1.0, 1.0, 1.0, 1.0);
		else return glm::vec4(0.0, 0.0, 0.0, 1.0);
	}));
	*/
	Billboard targetA(image::load("canyon.png"));
	Billboard targetB(WIDTH, HEIGHT, true, false); //color, no depth
	bool flip = true;

	Square2D flat;	//Flat square primitive for drawing billboard to screen

	//Shader for drawing billboard to screen and for doing an automata step
	Shader shader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	Shader automata({"shader/diffusion.vs", "shader/diffusion.fs"}, {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){

		if(!paused){
			if(flip){
				targetB.target(false);		//Use target as the render target (no clearing)!
				automata.use();																				//Use the Automata Shader
				automata.texture("imageTexture", targetA.texture);		//Target texture!
				automata.uniform("model", flat.model);
				flat.render();			//Render target texture to itself using primitive and automata shader
			}
			else{
				targetA.target(false);		//Use target as the render target (no clearing)!
				automata.use();																				//Use the Automata Shader
				automata.texture("imageTexture", targetB.texture);		//Target texture!
				automata.uniform("model", flat.model);
				flat.render();			//Render target texture to itself using primitive and automata shader
			}
			flip = !flip;
		}

		Tiny::view.target(color::black);									//Target screen
		shader.use(); 																		//Setup Shader
		if(flip) shader.texture("imageTexture", targetB.texture);
		else 		 shader.texture("imageTexture", targetA.texture);
		shader.texture("imageTexture", targetA.texture);
		shader.uniform("model", flat.model);
		flat.render();																		//Render Objects

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

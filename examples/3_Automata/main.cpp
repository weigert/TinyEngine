#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;															//Turn off VSYNC before opening window

	Tiny::window("Cellular Automata Example", 1200, 800);	//Open Window
	Tiny::event.handler = eventHandler;										//Define Event Handler
	Tiny::view.interface = [](){ /* ... */ }; 						//No Interface

	setup();	//Setup Model Data

	//Construct a billboard, using a texture generated from the raw data
	Billboard target(image::make<int>(glm::vec2(600, 400), data, [](int t){
		if(t == 0) return glm::vec4(1.0, 1.0, 1.0, 1.0);
		else return glm::vec4(0.0, 0.0, 0.0, 1.0);
	}));

	Square2D flat;	//Flat square primitive for drawing billboard to screen

	//Shader for drawing billboard to screen and for doing an automata step
	Shader shader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	Shader automata({"shader/automata.vs", "shader/automata.fs"}, {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){

		if(!paused){

			target.target();		//Use target as the render target!
			automata.use();																				//Use the Automata Shader
			automata.texture("imageTexture", target.texture);			//Target texture!
			automata.uniform("model", flat.model);
			flat.render();			//Render target texture to itself using primitive and automata shader

		}

		Tiny::view.target(color::black);									//Target screen
		shader.use(); 																		//Setup Shader
		shader.texture("imageTexture", target.texture);
		shader.uniform("model", flat.model);
		flat.render();																		//Render Objects

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

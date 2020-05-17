#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;

	Tiny::window("Cellular Automata Example", 1200, 800);

	Tiny::event.handler = eventHandler;

	Tiny::view.interface = [](){ /* ... */ }; //No Interface

	//Setup Model Data
	setup();

	Billboard target(image::make<int>(glm::vec2(600, 400), data, [](int t){
		if(t == 0) return glm::vec4(1.0, 1.0, 1.0, 1.0);
		else return glm::vec4(0.0, 0.0, 0.0, 1.0);
	}));

	Square2D flat;

	//Shader for drawing billboard to screen and for doing an automata step
	Shader shader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	Shader automata({"shader/automata.vs", "shader/automata.fs"}, {"in_Quad", "in_Tex"});

	//Render Pipeline
	Tiny::view.pipeline = [&](){

		if(!paused){

			target.target();

			automata.use();				//Use the Automata Shader
			automata.texture("imageTexture", target.texture.texture);
			automata.uniform("model", flat.model);

			flat.render();										//Render Field to Self

		}

		//Render Billboard to Screen
		Tiny::view.target(color::black);

		shader.use(); 										//Setup Shader
		shader.texture("imageTexture", target.texture.texture);
		shader.uniform("model", flat.model);

		flat.render();										//Render Objects

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

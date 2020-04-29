#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;

	Tiny::init("Cellular Automata Example", 1200, 800);

	Tiny::event.handler = eventHandler;

	Tiny::view.interface = [](){ /* ... */ }; //No Interface

	//Setup Model Data
	setup();

	//Utility Classes
	Billboard field(600, 400, false); //Render target for automata
	field.raw(image::make<int>(glm::vec2(600, 400), data, [](int t){
		if(t == 0) return glm::vec4(1.0, 1.0, 1.0, 1.0);
		else return glm::vec4(0.0, 0.0, 0.0, 1.0);
	}));	//Initialize raw texture data!

	//Shader for drawing billboard to screen and for doing an automata step
	Shader shader("shader/billboard.vs", "shader/billboard.fs", {"in_Quad", "in_Tex"});
	Shader automata("shader/automata.vs", "shader/automata.fs", {"in_Quad", "in_Tex"});

	//Render Pipeline
	Tiny::view.pipeline = [&](){

		if(!paused){

			//Render Billboard to Self for Automata Pass
			field.target();				//Render Target

			automata.use();				//Use the Automata Shader
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, field.texture);
			automata.uniform("imageTexture", 0);
			automata.uniform("model", field.model);

			field.render();										//Render Field to Self

		}

		//Render Billboard to Screen
		Tiny::view.target(color::black);

		shader.use(); 										//Setup Shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, field.texture);
		shader.uniform("imageTexture", 0);
		shader.uniform("model", field.model);

		field.render();										//Render Objects
	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

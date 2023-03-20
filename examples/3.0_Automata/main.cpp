#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;													//Turn off VSYNC before opening window
	bool paused = true;

	Tiny::window("GPU Accelerated Cellular Automata Example", 1200, 800);	//Open Window
	Tiny::event.handler = [&](){
	  if(!Tiny::event.press.empty())
	    if(Tiny::event.press.back() == SDLK_p)
	      paused = !paused;
	};
	Tiny::view.interface = [](){ /* ... */ }; 						//No Interface

	SDL_Surface* s = image::make([&](glm::ivec2 p){
			if(rand()%2) return glm::vec4(1.0, 1.0, 1.0, 1.0);
			else return glm::vec4(0.0, 0.0, 0.0, 1.0);
		}, glm::ivec2(1200, 800));

	Texture textureA(1200, 800, {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}, s->pixels);
	Texture textureB(1200, 800, {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE});

	Target targetA(1200, 800);
	targetA.bind(textureA, GL_COLOR_ATTACHMENT0);

	Target targetB(1200, 800);
	targetB.bind(textureB, GL_COLOR_ATTACHMENT0);

	bool flip = true;

	Square2D flat;	//Flat square primitive for drawing billboard to screen

	//Shader for drawing billboard to screen and for doing an automata step
	Shader shader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	Shader automata({"shader/automata.vs", "shader/automata.fs"}, {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){

		if(!paused){
			if(flip){
				targetB.target(false);		//Use target as the render target (no clearing)!
				automata.use();																				//Use the Automata Shader
				automata.texture("imageTexture", textureA);		//Target texture!
				automata.uniform("model", flat.model);
				flat.render();			//Render target texture to itself using primitive and automata shader
			}
			else{
				targetA.target(false);		//Use target as the render target (no clearing)!
				automata.use();																				//Use the Automata Shader
				automata.texture("imageTexture", textureB);		//Target texture!
				automata.uniform("model", flat.model);
				flat.render();			//Render target texture to itself using primitive and automata shader
			}
			flip = !flip;
		}

		Tiny::view.target(color::black);									//Target screen
		shader.use(); 																		//Setup Shader
		if(flip) shader.texture("imageTexture", textureA);
		else 		 shader.texture("imageTexture", textureB);
		shader.uniform("model", flat.model);
		flat.render();																		//Render Objects

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

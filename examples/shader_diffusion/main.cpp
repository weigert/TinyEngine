#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;															//Turn off VSYNC before opening window
	Tiny::window("GPU Accelerated Cellular Automata Example", WIDTH, HEIGHT);	//Open Window
  
	bool paused = true;
  Tiny::event.press[SDLK_p]([&paused](bool pressed){
    if(!pressed) paused = !paused;
  });
  
	setup();	//Setup Model Data

	Tiny::png image("canyon.png");
	Tiny::Texture textureA(image.width(), image.height(), Tiny::Texture::RGBA8U, image.data());

	Tiny::Texture textureB(1200, 800, {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE});

	Tiny::Target targetA(1200, 800);
	targetA.bind(textureA, GL_COLOR_ATTACHMENT0);

	Tiny::Target targetB(1200, 800);
	targetB.bind(textureB, GL_COLOR_ATTACHMENT0);

	bool flip = true;

	Tiny::Square2D flat;	//Flat square primitive for drawing billboard to screen

	//Shader for drawing billboard to screen and for doing an automata step
	Tiny::Shader shader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	Tiny::Shader automata({"shader/diffusion.vs", "shader/diffusion.fs"}, {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){

		if(!paused){
			if(flip){
				targetB.target();		//Use target as the render target (no clearing)!
				automata.use();																				//Use the Automata Shader
				automata.texture("imageTexture", textureA);		//Target texture!
				automata.uniform("model", glm::mat4(1.0f));
				flat.render();			//Render target texture to itself using primitive and automata shader
			}
			else{
				targetA.target();		//Use target as the render target (no clearing)!
				automata.use();																				//Use the Automata Shader
				automata.texture("imageTexture", textureB);		//Target texture!
				automata.uniform("model", glm::mat4(1.0f));
				flat.render();			//Render target texture to itself using primitive and automata shader
			}
			flip = !flip;
		}

		Tiny::view.target(color::black);									//Target screen
		shader.use(); 																		//Setup Shader
		if(flip) shader.texture("imageTexture", textureA);
		else 		 shader.texture("imageTexture", textureB);
		shader.uniform("model", glm::mat4(1.0f));
		flat.render();																		//Render Objects

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

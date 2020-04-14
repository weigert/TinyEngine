#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {
	srand(time(NULL));  //SEED
	
	Tiny::view.vsync = true;

	Tiny::init("Julia Set Explorer", 1000, 1000);

	Tiny::event.handler = eventHandler;
	Tiny::view.interface = interfaceFunc;

	//Utility Classes
	Billboard field(SIZE, SIZE, false); //Render target for automata
	field.raw(image::make<int>(glm::vec2(SIZE), data, [](int t){
		return glm::vec4(0.0, 0.0, 0.0, 1.0);
	}));	//Initialize raw texture data (just an empty billboard)!

	//Shader for drawing billboard to screen and for doing an automata step
	Shader julia("shader/julia.vs", "shader/julia.fs", {"in_Quad", "in_Tex"});

	//Render Pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);

		julia.use();				//Use the Automata Shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, field.texture);
		julia.setInt("imageTexture", 0);
		julia.setInt("maxiter", res);
		julia.setFloat("radius", radius);
		julia.setFloat("zoom", zoom);
		julia.setInt("order", order);
		julia.setVec4("converge", glm::vec4(cc[0], cc[1], cc[2], cc[3]));
		julia.setVec4("diverge", glm::vec4(dc[0], dc[1], dc[2], dc[3]));
		julia.setVec2("bias", glm::vec2(bias[0], bias[1]));
		julia.setVec2("offset", glm::vec2(px, py));
		julia.setMat4("model", field.model);

		field.render();										//Render Field to Self

	};

	Tiny::loop([&](){
		t += rate;		//Cycle
		if(t > 1.0) t -= 1.0;

		if(!active) return;	//Pause

		//Animate
		if(appear) radius = appearAmp*pow(sin(2*PI*t), 2);
		if(movex) bias[0] = center[0]+amplitude[0]*sin(2*PI*(t+phase[0]));
		if(movey) bias[1] = center[1]+amplitude[1]*sin(2*PI*(t+phase[1]));
	});

	Tiny::quit();

	return 0;
}

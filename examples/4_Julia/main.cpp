#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {
	srand(time(NULL));  //SEED

	Tiny::view.vsync = true;

	Tiny::window("Julia Set Explorer", 1000, 1000);

	Tiny::event.handler  = eventHandler;
	Tiny::view.interface = interfaceFunc;

	Square2D flat;												//Flat geometry primitive
	Shader julia({"shader/julia.vs", "shader/julia.fs"}, {"in_Quad", "in_Tex"});

	//Render Pipeline
	Tiny::view.pipeline = [&](){

		//Target screen for drawing
		Tiny::view.target(color::black);

		julia.use();
		julia.uniform("maxiter", res);
		julia.uniform("radius", radius);
		julia.uniform("zoom", zoom);
		julia.uniform("order", order);
		julia.uniform("converge", glm::vec4(cc[0], cc[1], cc[2], cc[3]));
		julia.uniform("diverge", glm::vec4(dc[0], dc[1], dc[2], dc[3]));
		julia.uniform("bias", glm::vec2(bias[0], bias[1]));
		julia.uniform("offset", glm::vec2(px, py));

		//Render to screen using flat geometry
		julia.uniform("model", flat.model);
		flat.render();

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

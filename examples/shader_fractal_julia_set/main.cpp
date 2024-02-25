#include <TinyEngine/TinyEngine>
#include <TinyEngine/gui>

#include "model.h"

int main( int argc, char* args[] ) {
	srand(time(NULL));  //SEED

	Tiny::view.vsync = true;

	Tiny::window("Julia Set Explorer", 1000, 1000);	//Open Window

	Tiny::event.scroll([](glm::ivec2 dir){
		if(dir.y >=  1) zoom *= 0.95;
		if(dir.y <= -1) zoom /= 0.95;
	});

  Tiny::event.active[SDLK_w]([](){
    py += 0.05*zoom;
  });

  Tiny::event.active[SDLK_a]([](){
    px += 0.05*zoom;
  });

  Tiny::event.active[SDLK_s]([](){
    py -= 0.05*zoom;
  });

  Tiny::event.active[SDLK_d]([](){
    px -= 0.05*zoom;
  });

	Tiny::GUI gui(interfaceFunc);
	gui.hook();

	Tiny::Square2D flat;												//Flat geometry primitive
	Tiny::Shader julia({"shader/julia.vs", "shader/julia.fs"}, {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));		//Target Screen

		julia.use();												//Prepare Shader
		julia.uniform("maxiter", res);
		julia.uniform("radius", radius);
		julia.uniform("zoom", zoom);
		julia.uniform("order", order);
		julia.uniform("converge", glm::vec4(cc[0], cc[1], cc[2], cc[3]));
		julia.uniform("diverge", glm::vec4(dc[0], dc[1], dc[2], dc[3]));
		julia.uniform("bias", glm::vec2(bias[0], bias[1]));
		julia.uniform("offset", glm::vec2(px, py));
		julia.uniform("model", glm::mat4(1));
		flat.render();											//Draw onto Flat Geometry

		gui.render();

	};

	Tiny::loop([&](){											//Extra Stuff (every tick)
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

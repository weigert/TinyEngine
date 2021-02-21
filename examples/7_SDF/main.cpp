#include <TinyEngine/TinyEngine>
#include <TinyEngine/color>

#include "model.h"

int main( int argc, char* args[] ) {
	srand(time(NULL));  //SEED

	Tiny::view.vsync = true;

	Tiny::window("2D Signed Distance Field", 1000, 1000);	//Open Window

	Tiny::event.handler  = eventHandler;		//Set Event Handler
	Tiny::view.interface = interfaceFunc;		//Set Interface Function

	Square2D flat;													//Flat geometry primitive
	Shader sdf({"shader/sdf.vs", "shader/sdf.fs"}, {"in_Quad", "in_Tex"});

	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);	//Target Screen

		sdf.use();												//Use the Shader
		sdf.uniform("zoom", zoom);
		sdf.uniform("converge", glm::vec4(cc[0], cc[1], cc[2], cc[3]));
		sdf.uniform("diverge", glm::vec4(dc[0], dc[1], dc[2], dc[3]));
		sdf.uniform("offset", glm::vec2(px, py));
		sdf.uniform("drawlines", drawlines);
		sdf.uniform("thickness", thickness);
		sdf.uniform("density", density);
		sdf.uniform("model", flat.model);
		flat.render();										//Render with Flat Geometry

	};

	Tiny::loop([&](){});								//Do Nothing

	Tiny::quit();

	return 0;
}

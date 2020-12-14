#include "../../TinyEngine.h"
#include "../../include/helpers/color.h"
#include "../../include/helpers/camera.h"

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::window("Heightmap Render", 1200, 800);	//Open Window
	cam::init();
	Tiny::event.handler = cam::handler;						//Event Handler
	Tiny::view.interface = [&](){ /* ... */ };		//No Interface

	setup();																					//Prepare Model Stuff

	Model mesh(_construct);														//Construct a Mesh
	mesh.shift(glm::vec3(-32.0, -15.0, -32.0));				//Translate Mesh
	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	Tiny::view.pipeline = [&](){											//Setup Drawing Pipeline

		Tiny::view.target(color::black);								//Target Screen

		defaultShader.use();														//Prepare Shader
		defaultShader.uniform("model", mesh.model);			//Set Model Matrix
		defaultShader.uniform("vp", cam::vp);	//View Projection Matrix
		mesh.render(GL_LINES);													//Render Model with Lines

	};

	Tiny::loop([&](){ //Autorotate Camera
		cam::pan(0.1f);
	});

	Tiny::quit();

	return 0;
}

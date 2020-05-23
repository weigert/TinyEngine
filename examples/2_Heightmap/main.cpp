#include "../../TinyEngine.h"
#include "../../include/helpers/color.h"

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::window("Heightmap Render", WIDTH, HEIGHT);	//Open Window
	Tiny::event.handler = eventHandler;								//Event Handler
	Tiny::view.interface = [&](){ /* ... */ };				//No Interface

	setup();																					//Prepare Model Stuff

	Model mesh(_construct);														//Construct a Mesh
	mesh.shift(glm::vec3(-32.0, -15.0, -32.0));				//Translate Mesh
	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	Tiny::view.pipeline = [&](){											//Setup Drawing Pipeline

		Tiny::view.target(color::black);								//Target Screen

		defaultShader.use();														//Prepare Shader
		defaultShader.uniform("model", mesh.model);			//Set Model Matrix
		defaultShader.uniform("vp", projection*camera);	//View Projection Matrix
		mesh.render(GL_LINES);													//Render Model with Lines

	};

	Tiny::loop([&](){ //Autorotate Camera
		camera = glm::rotate(camera, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
	});

	Tiny::quit();

	return 0;
}

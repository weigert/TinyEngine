#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	//Launch the Window
	Tiny::window("Heightmap Render", WIDTH, HEIGHT);

	//Add an Event Handler for camera movement
	Tiny::event.handler = eventHandler;

	//Set up an Interface
	Tiny::view.interface = [&](){ /* ... */

	};

	//Setup the Model Stuff
	setup();

	//Setup the Mesh and the Shader
	Model mesh(_construct);														//Construct a Mesh
	mesh.translate(glm::vec3(-32.0, -15.0, -32.0));		//Translate Mesh

	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		//Prepare Render Target
		Tiny::view.target(color::black);

		//Prepare Shader
		defaultShader.use();
		defaultShader.uniform("model", mesh.model);
		defaultShader.uniform("projectionCamera", projection*camera);

		//Render to Target
		mesh.render(GL_LINES);

	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

		//Autorotate Camera
		camera = glm::rotate(camera, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));

	});

	Tiny::quit();

	return 0;
}

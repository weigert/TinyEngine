#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	//Launch the Window
	Tiny::window("Particle System", WIDTH, HEIGHT);

	//Add an Event Handler for camera movement
	Tiny::event.handler = eventHandler;

	//Set up an Interface
	Tiny::view.interface = [&](){ /* ... */

	};

	//Setup the Model Stuff
	setup();

	//Setup the Particle System
	Particle particle;
	for(int i = 0; i < 1000; i++){
		//Random Positions and Rotations
		glm::mat4 model = glm::translate(glm::mat4(1.0),  glm::vec3(rand()%101-50, rand()%101-50, rand()%101-50));
		particle.models.push_back(glm::rotate(model, 2.0f*3.14159265f*(float)(rand()%360)/360.0f, glm::vec3(0.0, 1.0, 0.0)));
	}
	particle.update();

	//Setup a Texture to Draw
	Texture tex(image::load("dot.png"));

	//Setup the Particle Shader
	Shader particleShader("shader/particle.vs", "shader/particle.fs", {"in_Quad", "in_Tex", "in_Model"});

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		//Prepare Render Target
		Tiny::view.target(color::black);

		particleShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex.texture);
		particleShader.uniform("spriteTexture", 0);
		particleShader.uniform("projectionCamera", projection*camera);
		particle.render(); //Render Particle System

	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

		//Autorotate Camera
		camera = glm::rotate(camera, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));

	});

	Tiny::quit();

	return 0;
}

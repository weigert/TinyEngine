#include "../../TinyEngine.h"
#include "../../include/helpers/image.h"
#include "../../include/helpers/color.h"

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::window("Particle System", WIDTH, HEIGHT);
	Tiny::event.handler = eventHandler;
	Tiny::view.interface = [&](){ /* ... */ }; //No Interface

	setup(); 												//Prepare Model Stuff

	Square3D model;									//Model we want to instance render!

	std::vector<glm::mat4> models;
	for(int i = 0; i < 1000; i++){	//Generate random model matrices
		glm::mat4 model = glm::translate(glm::mat4(1.0),  glm::vec3(rand()%101-50, rand()%101-50, rand()%101-50));
		models.push_back(glm::rotate(model, 2.0f*3.14159265f*(float)(rand()%360)/360.0f, glm::vec3(0.0, 1.0, 0.0)));
	}
	Instance particle(&model);			//Particle system based on this model
	particle.addBuffer(models);			//Update particle system

	Texture tex(image::load("dot.png"));		//Texture to draw onto the model
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});

	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);

		particleShader.use();
		particleShader.texture("spriteTexture", tex);
		particleShader.uniform("projectionCamera", projection*camera);
		particle.render();

	};

	Tiny::loop([&](){ //Autorotate Camera
		camera = glm::rotate(camera, glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
	});

	Tiny::quit();

	return 0;
}

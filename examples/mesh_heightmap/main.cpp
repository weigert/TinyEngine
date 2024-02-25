#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#define GRIDSIZE 64

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::window("Heightmap Render", 1200, 800);			//Open Window

	Tiny::cam::orthogonal ortho({Tiny::view.WIDTH, Tiny::view.HEIGHT}, {-50.0f, 50.0f}, 10.0f);
	Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
	Tiny::camera cam(ortho, orbit);
	cam.hook();

	setup();																					//Prepare Model Stuff

	Tiny::Buffer positions, normals;							//Define Buffers
	Tiny::Buffer indices;
	construct(positions, normals, indices);						//Call algorithm to fill buffers

	Tiny::Indexed mesh({"in_Position", "in_Normal"}, indices);					//Create Model with 2 Properties
	mesh.bind<glm::vec3>("in_Position", positions);	//Bind Buffer to Property
	mesh.bind<glm::vec3>("in_Normal", normals);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-GRIDSIZE/2, -15.0, -GRIDSIZE/2));

	Tiny::Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	Tiny::view.pipeline = [&](){											//Setup Drawing Pipeline

		Tiny::view.target(glm::vec3(1));								//Target Screen

		defaultShader.use();														//Prepare Shader
		defaultShader.uniform("model", model);					//Set Model Matrix
		defaultShader.uniform("vp", cam.vp());						//View Projection Matrix
		mesh.render(GL_LINES);													//Render Model with Lines

	};

	Tiny::loop([&](){ //Autorotate Camera
	//	cam::pan(0.1f);
	});

	Tiny::quit();

	return 0;
}

#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.lineWidth = 1.0f;

	Tiny::window("Procedural Tree", WIDTH, HEIGHT);
	Tiny::event.handler = eventHandler;
	Tiny::view.interface = interfaceFunc;

	//Setup the Model Stuff
	setup();

	//Tree Mesh
	Model treemesh(_construct);														//Construct a Mesh

	//Leaves as Particles
	Square3D flat;															//Geometry for Particle System
	Particle particle(&flat);										//Make Particle System
	addLeaves<Square3D>(&particle);							//Generate the model matrices

	Texture tex(image::load("leaf.png"));

	//Setup the Particle Shader
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});
	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		//Prepare Render Target
		Tiny::view.target(glm::vec3(backcolor[0], backcolor[1], backcolor[2]));

		if(drawwire || drawtree){
			defaultShader.use();
			defaultShader.uniform("model", treemesh.model);
			defaultShader.uniform("projectionCamera", projection*camera);
			defaultShader.uniform("lightCol", glm::vec3(1.0));
			defaultShader.uniform("lookDir", lookPos - cameraPos);
			defaultShader.uniform("lightDir", lookPos - glm::vec3(250.0));
			defaultShader.uniform("lightStrength", 0.8f);

			if(drawtree){
				defaultShader.uniform("treecolor", glm::vec4(treecolor[0], treecolor[1], treecolor[2], treeopacity));
				defaultShader.uniform("wireframe", false);
				treemesh.render(GL_TRIANGLES);
			}

			if(drawwire){
				defaultShader.uniform("wirecolor", glm::vec3(wirecolor[0], wirecolor[1], wirecolor[2]));
				defaultShader.uniform("wireframe", true);
				treemesh.render(GL_LINES);
			}
		}

		if(drawleaf){
			particleShader.use();
			particleShader.texture("spriteTexture", tex.texture);
			particleShader.uniform("projectionCamera", projection*camera);
			particleShader.uniform("leafcolor", glm::vec4(leafcolor[0], leafcolor[1], leafcolor[2], leafopacity));

			particleShader.uniform("lightCol", glm::vec3(1.0));
			particleShader.uniform("lookDir", lookPos - cameraPos);
			particleShader.uniform("lightDir", lookPos - glm::vec3(250.0));
			particleShader.uniform("lightStrength", 0.8f);

			particle.render(GL_TRIANGLE_STRIP); //Render Particle System
		}
	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

		if(autorotate){
			camera = glm::rotate(camera, glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotation += 0.5f;
		}

		if(!paused)
			root->grow(growthrate);

		//Update Rendering Structures
		treemesh.construct(_construct);
		addLeaves<Square3D>(&particle);
		particle.update();

	});

	//Get rid of this thing!
	delete root;

	Tiny::quit();

	return 0;
}

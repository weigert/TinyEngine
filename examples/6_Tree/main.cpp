#include "../../TinyEngine.h"
#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.lineWidth = 1.5f;

	Tiny::init("Procedural Tree", WIDTH, HEIGHT);
	Tiny::event.handler = eventHandler;
	Tiny::view.interface = interfaceFunc;

	//Setup the Model Stuff
	setup();

	//Tree Mesh
	Model treemesh(_construct);														//Construct a Mesh

	//Leaves as Particles
	Particle particle;
	addLeaves(&particle);
	particle.update();

	//Setup a Texture to Draw
	Texture tex(image::load("leaf.png"));
	Texture normaltex(image::load("leafnormal.png"));

	//Setup the Particle Shader
	Shader particleShader("shader/particle.vs", "shader/particle.fs", {"in_Quad", "in_Tex", "in_Model"});
	Shader defaultShader("shader/default.vs", "shader/default.fs", {"in_Position", "in_Normal"});

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		//Prepare Render Target
		Tiny::view.target(glm::vec3(backcolor[0], backcolor[1], backcolor[2]));

		if(drawwire || drawtree){
			defaultShader.use();
			defaultShader.setMat4("model", treemesh.model);
			defaultShader.setMat4("projectionCamera", projection*camera);
			defaultShader.setVec3("lightCol", glm::vec3(1.0));
			defaultShader.setVec3("lookDir", lookPos - cameraPos);
			defaultShader.setVec3("lightDir", lookPos - glm::vec3(250.0));
			defaultShader.setFloat("lightStrength", 0.8f);

			if(drawtree){
				defaultShader.setVec4("treecolor", glm::vec4(treecolor[0], treecolor[1], treecolor[2], treeopacity));
				defaultShader.setBool("wireframe", false);
				treemesh.render(GL_TRIANGLES);
			}

			if(drawwire){
				defaultShader.setVec3("wirecolor", glm::vec3(wirecolor[0], wirecolor[1], wirecolor[2]));
				defaultShader.setBool("wireframe", true);
				treemesh.render(GL_LINES);
			}
		}

		if(drawleaf){
			particleShader.use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex.texture);
			particleShader.setInt("spriteTexture", 0);
			glActiveTexture(GL_TEXTURE0+1);
			glBindTexture(GL_TEXTURE_2D, normaltex.texture);
			particleShader.setInt("normalTexture", 1);
			particleShader.setMat4("projectionCamera", projection*camera);
			particleShader.setVec4("leafcolor", glm::vec4(leafcolor[0], leafcolor[1], leafcolor[2], leafopacity));
			
			particleShader.setVec3("lightCol", glm::vec3(1.0));
			particleShader.setVec3("lookDir", lookPos - cameraPos);
			particleShader.setVec3("lightDir", lookPos - glm::vec3(250.0));
			particleShader.setFloat("lightStrength", 0.8f);

			particle.render(); //Render Particle System
		}
	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

		if(autorotate){
			camera = glm::rotate(camera, glm::radians(0.5f), glm::vec3(0.0f, 1.0f, 0.0f));
			rotation += 0.5f;
		}

		if(!paused)
			tree.grow();

		treemesh.construct(_construct);
		addLeaves(&particle);
		particle.update();

	});

	Tiny::quit();

	return 0;
}

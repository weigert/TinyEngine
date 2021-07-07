#include "../TinyEngine.h"
#include "../../../include/helpers/image.h"
#include "../../../include/helpers/color.h"
#include "../../../include/helpers/helper.h"

#include <glm/gtc/type_ptr.hpp>

#include "model.h"

/*
* This function is necessary for Windows debug builds when
* compiling with Visual Studio 2019. It adds one 4x4 zero
* matrix to a vector of glm::mat4. Without this a vector of size 0 
* passed to glBufferData and glBufferSubData will throw an array 
* subscript error when trying to read the value of element 0. 
* 
* This doesn't happen on release builds, though I'm not sure
* why. Perhaps something to do with how the memory is initialised
* (or not initialised) for release builds. 
* 
* Also not sure why when this program is compiled with g++ on
* Linux the problem doesn't occur if the empty vector is passed in. 
*/
void initLeaves(std::vector<glm::mat4>& leaves)
{
	float vals[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	glm::mat4 newMat4 = glm::make_mat4(vals);
	leaves.push_back(newMat4);
}

int main( int argc, char* args[] ) {

	Tiny::view.lineWidth = 1.0f;

	Tiny::window("Procedural Tree", WIDTH, HEIGHT);
	Tiny::event.handler = eventHandler;
	Tiny::view.interface = interfaceFunc;

	setup();																				//Prepare Model Stuff

	Model treemesh(_construct);											//Construct a Mesh

	Square3D flat;																	//Geometry for Particle System

	std::vector<glm::mat4> leaves;
	addLeaves(leaves, true);												//Generate the model matrices

	Instance particle(&flat);												//Make Particle System
	
#if _DEBUG
	initLeaves(leaves);													// Necessary for windows debug builds
#endif
	particle.addBuffer(leaves);											//Add Matrices

	Texture tex(image::load("leaf.png"));

	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});
	Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});
	Shader depth({"shader/depth.vs", "shader/depth.fs"}, {"in_Position"});
	Shader particledepth({"shader/particledepth.vs", "shader/particledepth.fs"}, {"in_Quad", "in_Tex", "in_Model"});
	Billboard shadow(1600, 1600, false); 						//No Color Buffer

	Model floor(construct_floor);
	floor.move(glm::vec3(0), 0, glm::vec3(1000));	//So we can cast shadows

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		shadow.target();
		if(drawshadow){
			depth.use();
			depth.uniform("dvp", lproj*lview);
			defaultShader.uniform("model", treemesh.model);
			treemesh.render(GL_TRIANGLES);
		}
		if(leafshadow){
			particledepth.use();
			particledepth.uniform("dvp", lproj*lview);
			particledepth.texture("spriteTexture", tex);
			addLeaves(leaves, false);						//Generate the model matrices
			
#if _DEBUG
			if (leaves.size() == 0)
				initLeaves(leaves);													// Necessary for windows debug builds
#endif
			particle.updateBuffer(leaves, 0);

			particle.render(GL_TRIANGLE_STRIP); 		//Render Particle System
		}

		//Prepare Render Target
		Tiny::view.target(glm::vec3(backcolor[0], backcolor[1], backcolor[2]));

		if(drawwire || drawtree){
			defaultShader.use();
			defaultShader.uniform("model", treemesh.model);
			defaultShader.uniform("projectionCamera", projection*camera);
			defaultShader.uniform("lightcolor", lightcolor);
			defaultShader.uniform("lookDir", lookPos - cameraPos);
			defaultShader.uniform("lightDir", lightpos);

			defaultShader.uniform("drawshadow", drawshadow);
			if(drawshadow){
				defaultShader.uniform("dbvp", bias*lproj*lview);
				defaultShader.texture("shadowMap", shadow.depth);
				defaultShader.uniform("light", lightpos);
			}

			defaultShader.uniform("drawfloor", true);
			defaultShader.uniform("drawcolor", glm::vec4(backcolor[0],backcolor[1],backcolor[2],1));
			defaultShader.uniform("model", floor.model);
			floor.render();
			defaultShader.uniform("drawfloor", false);

			defaultShader.uniform("model", treemesh.model);

			if(drawtree){
				defaultShader.uniform("drawcolor", glm::vec4(treecolor[0], treecolor[1], treecolor[2], treeopacity));
				defaultShader.uniform("wireframe", false);
				treemesh.render(GL_TRIANGLES);
			}

			if(drawwire){
				defaultShader.uniform("drawcolor", glm::vec4(wirecolor[0], wirecolor[1], wirecolor[2], 1.0));
				defaultShader.uniform("wireframe", true);
				treemesh.render(GL_LINES);
			}
		}

		if(drawleaf){
			particleShader.use();
			particleShader.texture("spriteTexture", tex);
			particleShader.uniform("projectionCamera", projection*camera);
			particleShader.uniform("leafcolor", glm::vec4(leafcolor[0], leafcolor[1], leafcolor[2], leafopacity));
			particleShader.uniform("lightcolor", lightcolor);

			particleShader.uniform("selfshadow", selfshadow);
			if(selfshadow){
				particleShader.uniform("dbvp", bias*lproj*lview);
				particleShader.texture("shadowMap", shadow.depth);
				particleShader.uniform("light", lightpos);
			}

			particleShader.uniform("lookDir", lookPos - cameraPos);
			addLeaves(leaves, true);
			
#if _DEBUG
			if (leaves.size() == 0)
				initLeaves(leaves);													// Necessary for windows debug builds
#endif
			particle.updateBuffer(leaves, 0);
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
		particle.updateBuffer(leaves, 0);

	});

	//Get rid of this thing!
	delete root;

	Tiny::quit();

	return 0;
}

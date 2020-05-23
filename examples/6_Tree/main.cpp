#include "../../TinyEngine.h"
#include "../../include/helpers/image.h"
#include "../../include/helpers/color.h"
#include "../../include/helpers/helper.h"

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.lineWidth = 1.0f;

	Tiny::window("Procedural Tree", WIDTH, HEIGHT);
	Tiny::event.handler = eventHandler;
	Tiny::view.interface = interfaceFunc;

	setup();																				//Prepare Model Stuff

	Model treemesh(_construct);											//Construct a Mesh

	Square3D flat;																	//Geometry for Particle System
	Particle particle(&flat);												//Make Particle System
	addLeaves<Square3D>(&particle, true);						//Generate the model matrices

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
			addLeaves<Square3D>(&particle, false);	//Generate the model matrices
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
			addLeaves<Square3D>(&particle, true);
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
		particle.update();

	});

	//Get rid of this thing!
	delete root;

	Tiny::quit();

	return 0;
}

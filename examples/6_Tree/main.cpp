#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>
#include <TinyEngine/helper>
#include <TinyEngine/camera>

#define PI 3.14159265f

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.lineWidth = 1.0f;

	Tiny::window("Procedural Tree", 1200, 800);
	cam::look = glm::vec3(0, 100, 0);
	cam::far = 2000.0f;
	cam::roty = 25.0f;
	cam::zoomrate = 10.0f;
	cam::init(600, cam::PROJ);

	bool paused = false;
	bool autorotate = true;

	Tiny::event.handler = [&](){
	  (cam::handler)();
	  if(!Tiny::event.press.empty()){
	    if(Tiny::event.press.back() == SDLK_p)
	      paused = !paused;
	    else if(Tiny::event.press.back() == SDLK_a)
	      autorotate = !autorotate;
	    else if(Tiny::event.press.back() == SDLK_r){
	      Branch* newroot = new Branch(root, true);
	      delete(root);
	      root = newroot;
	    }
	  }
	};

	Tiny::view.interface = interfaceFunc;

	root = new Branch({0.6, 0.45, 2.5}); //Create Root

	Model treemesh(_construct);											//Construct a Mesh

	Square3D flat;																	//Geometry for Particle System

	std::vector<glm::mat4> leaves;
	addLeaves(leaves, true);												//Generate the model matrices

	Instance particle(&flat);												//Make Particle System
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
			particle.updateBuffer(leaves, 0);

			particle.render(GL_TRIANGLE_STRIP); 		//Render Particle System
		}

		//Prepare Render Target
		Tiny::view.target(glm::vec3(backcolor[0], backcolor[1], backcolor[2]));

		if(drawwire || drawtree){
			defaultShader.use();
			defaultShader.uniform("model", treemesh.model);
			defaultShader.uniform("projectionCamera", cam::vp);
			defaultShader.uniform("lightcolor", lightcolor);
			defaultShader.uniform("lookDir", cam::look - cam::pos);
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
			particleShader.uniform("projectionCamera", cam::vp);
			particleShader.uniform("leafcolor", glm::vec4(leafcolor[0], leafcolor[1], leafcolor[2], leafopacity));
			particleShader.uniform("lightcolor", lightcolor);

			particleShader.uniform("selfshadow", selfshadow);
			if(selfshadow){
				particleShader.uniform("dbvp", bias*lproj*lview);
				particleShader.texture("shadowMap", shadow.depth);
				particleShader.uniform("light", lightpos);
			}

			particleShader.uniform("lookDir", cam::look - cam::pos);
			addLeaves(leaves, true);
			particle.updateBuffer(leaves, 0);
			particle.render(GL_TRIANGLE_STRIP); //Render Particle System
		}
	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

		if(autorotate)
			cam::pan(0.5f);

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

#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>
#include <TinyEngine/math>
#include <TinyEngine/camera>

#define PI 3.14159265f

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::window("Procedural Tree", 1200, 800);

	Tiny::cam::ortho ortho(Tiny::view.WIDTH, Tiny::view.HEIGHT, -200.0f, 200.0f, 10.0f);
	Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
	ortho.update();
	orbit.update();

	Tiny::cam::camera cam(ortho, orbit);
	/*

	cam::look = glm::vec3(0, 100, 0);
	cam::far = 2000.0f;
	cam::roty = 25.0f;
	cam::zoomrate = 10.0f;
	cam::init(600, cam::PROJ);
*/

	bool paused = false;
	bool autorotate = true;

	Tiny::event.handler = [&](){
	  (cam.handler)();
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

	Tiny::Buffer positions, normals;
	Tiny::Buffer colors;
	Tiny::Buffer indices;
	construct(positions, normals, colors, indices);

	Tiny::Indexed treemesh({"in_Position", "in_Normal", "in_Color"}, indices);
	treemesh.bind<glm::vec3>("in_Position", positions);
	treemesh.bind<glm::vec3>("in_Normal", normals);
	treemesh.bind<glm::vec4>("in_Color", colors);
	treemesh.set(indices);

	Tiny::Square3D flat;																	//Geometry for Particle System

	std::vector<glm::mat4> leaves;
	addLeaves(leaves, true);												//Generate the model matrices

	Tiny::Buffer models(leaves);
	Tiny::Instance particle(flat);												//Make Particle System
	particle.bind<glm::mat4>(models);  //Add Matrices

	Tiny::png image("leaf");
	Tiny::Texture tex(image.width(), image.height(), Tiny::Texture::RGBA8U, image.data());

	Tiny::Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});
	Tiny::Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});
	Tiny::Shader depth({"shader/depth.vs", "shader/depth.fs"}, {"in_Position"});
	Tiny::Shader particledepth({"shader/particledepth.vs", "shader/particledepth.fs"}, {"in_Quad", "in_Tex", "in_Model"});

	Tiny::Target shadow(1600, 1600);
	Tiny::Texture depthtex(1600, 1600, {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE});
	shadow.bind(depthtex, GL_DEPTH_ATTACHMENT);

	Tiny::Square3D floor;
	glm::mat4 floormodel = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1,0,0));
	floormodel = glm::scale(floormodel, glm::vec3(1000));

	Tiny::view.pipeline = [&](){	//Setup Drawing Pipeline

		shadow.target();
		if(drawshadow){
			depth.use();
			depth.uniform("dvp", lproj*lview);
			defaultShader.uniform("model", glm::mat4(1.0f));
			treemesh.render(GL_TRIANGLES);
		}
		if(leafshadow){
			particledepth.use();
			particledepth.uniform("dvp", lproj*lview);
			particledepth.texture("spriteTexture", tex);
			addLeaves(leaves, false);						//Generate the model matrices
			models.set<glm::mat4>(leaves);

			particle.render(GL_TRIANGLE_STRIP, leaves.size()); 		//Render Particle System
		}

		//Prepare Render Target
		Tiny::view.target(glm::vec3(backcolor[0], backcolor[1], backcolor[2]));

		if(drawwire || drawtree){
			defaultShader.use();
			defaultShader.uniform("model", glm::mat4(1.0f));
			defaultShader.uniform("projectionCamera", cam.vp());
			defaultShader.uniform("lightcolor", lightcolor);
			defaultShader.uniform("lookDir", cam.control.look() - cam.control.pos());
			defaultShader.uniform("lightDir", lightpos);

			defaultShader.uniform("drawshadow", drawshadow);
			if(drawshadow){
				defaultShader.uniform("dbvp", bias*lproj*lview);
				defaultShader.texture("shadowMap", depthtex);
				defaultShader.uniform("light", lightpos);
			}

			defaultShader.uniform("drawfloor", true);
			defaultShader.uniform("drawcolor", glm::vec4(backcolor[0],backcolor[1],backcolor[2],1));
			defaultShader.uniform("model", floormodel);
			floor.render();
			defaultShader.uniform("drawfloor", false);

			defaultShader.uniform("model", glm::mat4(1.0f));

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
			particleShader.uniform("projectionCamera", cam.vp());
			particleShader.uniform("ff", glm::rotate(glm::mat4(1.0f), glm::radians(180-cam.control._phi()), glm::vec3(0,1,0)));
			particleShader.uniform("leafcolor", glm::vec4(leafcolor[0], leafcolor[1], leafcolor[2], leafopacity));
			particleShader.uniform("lightcolor", lightcolor);

			particleShader.uniform("selfshadow", selfshadow);
			if(selfshadow){
				particleShader.uniform("dbvp", bias*lproj*lview);
				particleShader.texture("shadowMap", depthtex);
				particleShader.uniform("light", lightpos);
			}

			particleShader.uniform("lookDir", cam.control.look() - cam.control.pos());
			addLeaves(leaves, true);
			models.set<glm::mat4>(leaves);
		//	particle.SIZE = leaves.size();
			particle.render(GL_TRIANGLE_STRIP, leaves.size()); //Render Particle System

		}

	};

	//Loop over Stuff
	Tiny::loop([&](){ /* ... */

		if(autorotate)
			cam.control.pan(0.05f);

		if(!paused)
			root->grow(growthrate);

		//Update Rendering Structures
		construct(positions, normals, colors, indices);
		treemesh.set(indices);

		models.set<glm::mat4>(leaves);

	});

	//Get rid of this thing!
	delete root;

	Tiny::quit();

	return 0;
}

#include "../TinyEngine.h"
#include "../../../include/helpers/color.h"
#include "../../../include/helpers/object.h"

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = true;
	Tiny::window("Lighting Scene Test", 1200, 800);

	setup();

	Tiny::event.handler  = eventHandler;
	Tiny::view.interface = interfaceFunc;

	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});
	Shader cubedepth({"shader/cubedepth.vs", "shader/cubedepth.gs", "shader/cubedepth.fs"}, {"in_Position"});

	std::string path = "object/";
	Model lamp(obj::load, path+"Lamp");
	Model table(obj::load, path+"Table");
	Model chair(obj::load, path+"Chair");
	Model shelf(obj::load, path+"Shelf");
	Model frame(obj::load, path+"Frame");
	table.shift(glm::vec3(0, 0,  2));
	chair.shift(glm::vec3(0, 0, -2));
	lamp.shift(glm::vec3(2, 5, 2));
	shelf.shift(glm::vec3( 24, 0, 0));
	frame.shift(glm::vec3(-25, 15, 0), 90);

	Model room(construct_room);
	room.move(glm::vec3(0), 0, glm::vec3(25)); //Scale Floor

	//Shadow Map
	Cubemap pointshadow(1000, 1000, false); //No Color Buffer

	Tiny::view.pipeline = [&](){

		//Point Shadow Map
		if(lightupdate){ //Only do it if necessary
			pointshadow.target();
			cubedepth.use();
			cubedepth.uniform("vp", views);
			cubedepth.uniform("proj", pointproj);
			cubedepth.uniform("lightPos", plightpos);
			cubedepth.uniform("far", pointfar);
			cubedepth.uniform("model", lamp.model);
			lamp.render();
			cubedepth.uniform("model", table.model);
			table.render();
			cubedepth.uniform("model", chair.model);
			chair.render();
			cubedepth.uniform("model", shelf.model);
			shelf.render();
			cubedepth.uniform("model", frame.model);
		  frame.render();
			lightupdate = false;
		}

		//Render Scene
		Tiny::view.target(color::black);	//Target Screen

		shader.use();
		shader.uniform("vp", proj*view);
		shader.uniform("camera", pos);

		//Point Shadow
		shader.texture("pointshadowMap", pointshadow.depth);
		shader.uniform("pointlightpos", plightpos);
		shader.uniform("pointlightfar", pointfar);
		shader.uniform("pointlighton", on);
		shader.uniform("pointlightcolor", lightcolor);
		shader.uniform("brightness", brightness);
		shader.uniform("attenuation", attenuation);

		//Draw Models
		shader.uniform("model", room.model);
		room.render();
		shader.uniform("model", lamp.model);
		lamp.render();
		shader.uniform("model", table.model);
		table.render();
		shader.uniform("model", chair.model);
		chair.render();
		shader.uniform("model", shelf.model);
		shelf.render();
		shader.uniform("model", frame.model);
	  frame.render();
	};

	Tiny::loop([&](){

	});

	Tiny::quit();

	return 0;
}

#include <TinyEngine/TinyEngine>
#include <TinyEngine/color>
#include <TinyEngine/object>

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = true;
	Tiny::window("Lighting Scene Test", 1200, 800);

	setup();

	Tiny::event.handler  = eventHandler;
	Tiny::view.interface = interfaceFunc;

	Tiny::Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});
	Tiny::Shader cubedepth({"shader/cubedepth.vs", "shader/cubedepth.gs", "shader/cubedepth.fs"}, {"in_Position"});

	std::string path = "object/";

	Tiny::Object lamp(path+"Lamp");
	Tiny::Object chair(path+"Chair");
	Tiny::Object table(path+"Table");
	Tiny::Object shelf(path+"Shelf");
	Tiny::Object frame(path+"Frame");

	glm::mat4 lampmodel = glm::translate(glm::mat4(1.0f), glm::vec3(2,5,2));
	glm::mat4 tablemodel = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,2));
	glm::mat4 chairmodel = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-2));
	glm::mat4 shelfmodel = glm::translate(glm::mat4(1.0f), glm::vec3(24,0,0));
	glm::mat4 framemodel = glm::translate(glm::mat4(1.0f), glm::vec3(-25,15,0));
	framemodel = glm::rotate(framemodel, glm::radians(90.0f), glm::vec3(0,1,0));

  Tiny::Indexed* room = construct_room();
	glm::mat4 roommodel = glm::scale(glm::mat4(1.0f), glm::vec3(25));

	//Shadow Map
	Tiny::CubeMap pointshadow(1000, 1000);

	Tiny::view.pipeline = [&](){

		//Point Shadow Map
		if(lightupdate){ //Only do it if necessary

			pointshadow.clear();
			pointshadow.target();
			cubedepth.use();
			cubedepth.uniform("vp", views);
			cubedepth.uniform("proj", pointproj);
			cubedepth.uniform("lightPos", plightpos);
			cubedepth.uniform("far", pointfar);

			cubedepth.uniform("model", lampmodel);
			lamp.render(GL_TRIANGLES, lamp.size());
			cubedepth.uniform("model", tablemodel);
			table.render(GL_TRIANGLES, table.size());
			cubedepth.uniform("model", chairmodel);
			chair.render(GL_TRIANGLES, chair.size());
			cubedepth.uniform("model", shelfmodel);
			shelf.render(GL_TRIANGLES, shelf.size());
			cubedepth.uniform("model", framemodel);
			frame.render(GL_TRIANGLES, frame.size());

			lightupdate = false;
		}

		//Render Scene
		Tiny::view.target(color::black);	//Target Screen

		shader.use();
		shader.uniform("vp", proj*view);
		shader.uniform("camera", pos);

		//Point Shadow
		shader.texture("pointshadowMap", pointshadow.depth());
		shader.uniform("pointlightpos", plightpos);
		shader.uniform("pointlightfar", pointfar);
		shader.uniform("pointlighton", on);
		shader.uniform("pointlightcolor", lightcolor);
		shader.uniform("brightness", brightness);
		shader.uniform("attenuation", attenuation);

		shader.uniform("model", roommodel);
		room->render(GL_TRIANGLES);

		shader.uniform("model", lampmodel);
		lamp.render(GL_TRIANGLES, lamp.size());
		shader.uniform("model", tablemodel);
		table.render(GL_TRIANGLES, table.size());
		shader.uniform("model", chairmodel);
		chair.render(GL_TRIANGLES, chair.size());
		shader.uniform("model", shelfmodel);
		shelf.render(GL_TRIANGLES, shelf.size());
		shader.uniform("model", framemodel);
		frame.render(GL_TRIANGLES, frame.size());

	};

	Tiny::loop([&](){

	});

	Tiny::quit();

	return 0;
}

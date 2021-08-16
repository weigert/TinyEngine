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

	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});
	Shader cubedepth({"shader/cubedepth.vs", "shader/cubedepth.gs", "shader/cubedepth.fs"}, {"in_Position"});

	std::string path = "object/";

	Model* lamp = obj::load(path+"Lamp");
	Model* chair = obj::load(path+"Chair");
	Model* table = obj::load(path+"Table");
	Model* shelf =  obj::load(path+"Shelf");
	Model* frame =  obj::load(path+"Frame");

	lamp->model = glm::translate(glm::mat4(1.0f), glm::vec3(2,5,2));
	table->model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,2));
	chair->model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-2));
	shelf->model = glm::translate(glm::mat4(1.0f), glm::vec3(24,0,0));
	frame->model = glm::translate(glm::mat4(1.0f), glm::vec3(-25,15,0));
	frame->model = glm::rotate(frame->model, glm::radians(90.0f), glm::vec3(0,1,0));

  Model* room = construct_room();
	room->model = glm::scale(glm::mat4(1.0f), glm::vec3(25));

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

			cubedepth.uniform("model", lamp->model);
			lamp->render(GL_TRIANGLES);
			cubedepth.uniform("model", table->model);
			table->render(GL_TRIANGLES);
			cubedepth.uniform("model", chair->model);
			chair->render(GL_TRIANGLES);
			cubedepth.uniform("model", shelf->model);
			shelf->render(GL_TRIANGLES);
			cubedepth.uniform("model", frame->model);
			frame->render(GL_TRIANGLES);

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

		shader.uniform("model", room->model);
		room->render(GL_TRIANGLES);

		shader.uniform("model", lamp->model);
		lamp->render(GL_TRIANGLES);
		shader.uniform("model", table->model);
		table->render(GL_TRIANGLES);
		shader.uniform("model", chair->model);
		chair->render(GL_TRIANGLES);
		shader.uniform("model", shelf->model);
		shelf->render(GL_TRIANGLES);
		shader.uniform("model", frame->model);
		frame->render(GL_TRIANGLES);

	};

	Tiny::loop([&](){

	});

	Tiny::quit();

	return 0;
}

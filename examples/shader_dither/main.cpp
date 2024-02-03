#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>
#include <TinyEngine/object>
#include <TinyEngine/camera>

const int WIDTH = 800;
const int HEIGHT = 800;

int main( int argc, char* args[] ) {

	Tiny::window("Dithering Shader", WIDTH, HEIGHT);	//Open Window

  Tiny::cam::ortho ortho(Tiny::view.WIDTH, Tiny::view.HEIGHT, -100.0f, 100.0f, 50.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
  ortho.update();
  orbit.update();

  Tiny::cam::camera cam(ortho, orbit);
  cam.update();

	bool paused = false;

	Tiny::event.handler = [&](){
		cam.handler();
		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;
	};

	int pres = 4; //Pixel Resolution
	int cres = 8; //Color Resolution

	Tiny::view.interface = [&](){
	  ImGui::Text("Dither Controller");
	  ImGui::DragInt("Pixel Resolution", &pres, 1, 1, 64);
	  ImGui::DragInt("Color Resolution", &cres, 1, 1, 256);
	};

	std::string path = "skull";
	Tiny::Object skull(path);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0,-3.5,0));

	Tiny::Square2D flat;	//Flat square primitive for drawing billboard to screen
	Tiny::Shader dither({"shader/dither.vs", "shader/dither.fs"}, {"in_Quad", "in_Tex"});
	Tiny::Shader basic({"shader/default.vs", "shader/default.fs"}, {"in_Quad", "in_Tex"});

	Tiny::Billboard image(WIDTH, HEIGHT);

	Tiny::view.pipeline = [&](){

		image.clear(glm::vec4(0,0,0,1));
		image.target();

		basic.use();
		basic.uniform("model", model);
		basic.uniform("vp", cam.vp());

		skull.render(GL_TRIANGLES, skull.size());

		Tiny::view.target(color::black);									//Target screen

		dither.use(); 																		//Setup Shader
		dither.texture("imageTexture", image.color());
		dither.uniform("model", glm::mat4(1.0f));
		dither.uniform("pres", pres);
		dither.uniform("cres", cres);
		flat.render();																		//Render Objects

	};

	Tiny::loop([&](){
		if(!paused) 
			cam.control.pan(0.01f);
	});

	Tiny::quit();

	return 0;
}

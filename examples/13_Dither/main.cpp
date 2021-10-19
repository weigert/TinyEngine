#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>
#include <TinyEngine/object>
#include <TinyEngine/camera>

const int WIDTH = 800;
const int HEIGHT = 800;

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;															//Turn off VSYNC before opening window
	bool paused = false;

	Tiny::window("Dithering Shader", WIDTH, HEIGHT);	//Open Window
	cam::init();
	Tiny::event.handler = [&](){
		(cam::handler)();
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
	Model* skull = obj::load(path);
	skull->model = glm::translate(glm::mat4(1.0f), glm::vec3(0,-3.5,0));

	Square2D flat;	//Flat square primitive for drawing billboard to screen
	Shader dither({"shader/dither.vs", "shader/dither.fs"}, {"in_Quad", "in_Tex"});
	Shader basic({"shader/default.vs", "shader/default.fs"}, {"in_Quad", "in_Tex"});

	Billboard image(WIDTH, HEIGHT);

	Tiny::view.pipeline = [&](){

		image.target(color::black);

		basic.use();
		basic.uniform("model", skull->model);
		basic.uniform("vp", cam::vp);

		skull->render(GL_TRIANGLES);

		Tiny::view.target(color::black);									//Target screen

		dither.use(); 																		//Setup Shader
		dither.texture("imageTexture", image.texture);
		dither.uniform("model", flat.model);
		dither.uniform("pres", pres);
		dither.uniform("cres", cres);
		flat.render();																		//Render Objects

	};

	Tiny::loop([&](){
		if(!paused) cam::pan(0.1f);
	});

	Tiny::quit();

	return 0;
}

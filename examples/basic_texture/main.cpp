#include <TinyEngine/TinyEngine>

#include <TinyEngine/image>
#include <TinyEngine/color>
#include <TinyEngine/gui>

int main( int argc, char* args[] ) {

	Tiny::window("Shader Effects Example", 1200, 800);

	int ind = 0;    //Effect Index
	int res = 100;  //Pixelate Effect
	int bits = 4;   //Bitreduce Effect

	Tiny::GUI gui([&](){
	  ImGui::Text("Shader Effects");
	  ImGui::DragInt("Effect", &ind, 1, 0, 2);
	  ImGui::DragInt("Resolution", &res, 1, 1, 400);
	  ImGui::DragInt("Bits", &bits, 1, 1, 16);
	});
	gui.hook();

	Tiny::png image("canyon.png");
	Tiny::Texture tex(image.width(), image.height(), Tiny::Texture::RGBA8U, image.data());
	Tiny::Square2D flat;														//Create Primitive Model
	Tiny::Shader effect({"shader/effect.vs", "shader/effect.fs"}, {"in_Quad", "in_Tex"});

	std::string test = "imageTexture";

	Tiny::view.pipeline = [&](){

		Tiny::view.target(color::black);				//Target Main Screen

		effect.use();														//Use Effect Shader
		effect.uniform("index", ind);						//Define Uniforms
		effect.uniform("res", res);
		effect.uniform("bits", bits);
		effect.texture("imageTexture", tex);		//Load Texture
		effect.uniform("model", glm::mat4(1));	//Add Model Matrix
		flat.render();													//Render Primitive
	
		gui.render();

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

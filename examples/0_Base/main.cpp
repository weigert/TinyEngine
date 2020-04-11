#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	//Add the Event Handler
	Tiny::event.handler = [&](){ /*...*/ };

	//Set up an Interface
	Tiny::view.interface = [&](){
		ImGui::SetNextWindowSize(ImVec2(450, 280), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(50, 470), ImGuiCond_Once);

		ImGui::Begin("Test Controller");
			ImGui::Text("Test");
		ImGui::End();
	};

	//Launch the Window
	Tiny::init("Test Window", 1200, 800);

	/* Setup your Data here! */
	Shader billboardShader("../shader/billboard.vs", "../shader/billboard.fs", {"in_Quad", "in_Tex"});

	/* Billboard Setup needs Improvement */
	Billboard image("hunter.png");
	image.move(glm::vec2(0), glm::vec2(0.2));

	//Drawing Pipeline
	Tiny::view.pipeline = [&](){

		//Draw the Billboard here!
		billboardShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, image.texture);
		billboardShader.setInt("imageTexture", 0);
		billboardShader.setMat4("model", image.model);
		image.render();

	};

	//Loop over Stuff
	Tiny::loop([&](){

		/* Do whatever you want here... */

	});

	Tiny::quit();

	return 0;
}

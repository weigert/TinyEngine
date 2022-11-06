#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <TinyEngine/color>

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;													//Turn off VSYNC before opening window
	bool paused = true;

	Tiny::window("GPU Accelerated Cellular Automata Example", 600, 400);	//Open Window
	Tiny::event.handler = [&](){
	  if(!Tiny::event.press.empty())
	    if(Tiny::event.press.back() == SDLK_p)
	      paused = !paused;
	};
	Tiny::view.interface = [](){ /* ... */ }; 						//No Interface


	Square2D flat;	//Flat square primitive for drawing billboard to screen

	//Shader for drawing billboard to screen and for doing an automata step
	Shader point({"shader/point.vs", "shader/point.fs"}, {"in_Quad", "in_Tex"});

	//Shader for drawing billboard to screen and for doing an automata step
	Shader shader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	Shader decay({"shader/decay.vs", "shader/decay.fs"}, {"in_Quad", "in_Tex"});

	Shader diffuse({"shader/diffusion.vs", "shader/diffusion.fs"}, {"in_Quad", "in_Tex"});

	std::vector<glm::vec2> pos;		// Position
	std::vector<glm::vec2> speed;	// Speed

	int N = 100000;	// Anzahl, 1 Millionen

	for(size_t n = 0; n < N; n++){

		glm::vec2 nn = (glm::vec2(rand()%1000, rand()%1000) - glm::vec2(500))/500.0f;
		glm::vec2 pn = (glm::vec2(rand()%1000, rand()%1000) - glm::vec2(500))/500.0f;

		if(glm::length(nn) > 0.1){

			pos.push_back(pn);
			speed.push_back(glm::normalize(nn));

		}

	}

	N = pos.size();


	Buffer posbuf(pos);
	Buffer speedbuf(speed);

	Compute shift({"shader/shift.cs"}, {"position", "speed"});
	shift.bind<glm::vec2>("position", &posbuf);
	shift.bind<glm::vec2>("speed", &speedbuf);

	Model pointmesh({"in_Position"});
	pointmesh.bind<glm::vec2>("in_Position", &posbuf);
	pointmesh.SIZE = posbuf.SIZE;

	Billboard targetA(600, 400, true, false);
	Billboard targetB(600, 400, true, false);


	bool flip = true;

	Tiny::view.pipeline = [&](){

		shift.use();
		shift.uniform("N", N);

		if(flip){
			shift.texture("imageTexture", targetB.texture);
		}
		else{
			shift.texture("imageTexture", targetA.texture);
		}

		shift.dispatch(1+N/1024);


		point.use();																										//Setup Shader

		if(flip){
			targetA.target(false);
			point.texture("imageTexture", targetB.texture);
		}
		else{
			targetB.target(false);
			point.texture("imageTexture", targetA.texture);
		}

		pointmesh.render(GL_POINTS);																		//Render Objects



		// Diffuse

		diffuse.use();
		diffuse.uniform("model", flat.model);

		if(flip){

			targetB.target(color::black);
			diffuse.texture("imageTexture", targetA.texture);
			flat.render();

			targetA.target(color::black);
			diffuse.texture("imageTexture", targetB.texture);
			flat.render();

			targetB.target(color::black);
			diffuse.texture("imageTexture", targetA.texture);
			flat.render();

			targetA.target(color::black);
			diffuse.texture("imageTexture", targetB.texture);
			flat.render();

		}





		decay.use();
		decay.uniform("model", flat.model);

		if(flip){
			targetB.target(color::black);
			decay.texture("imageTexture", targetA.texture);
		}
		else {
			targetA.target(color::black);
			decay.texture("imageTexture", targetB.texture);
		}
		flat.render();



		Tiny::view.target(color::black);									//Target screen
		shader.use();
		shader.uniform("model", flat.model);
		if(flip){
			shader.texture("imageTexture", targetB.texture);
		}
		else {
			shader.texture("imageTexture", targetA.texture);
		}
		flat.render();

		flip = !flip;

	};

	Tiny::loop([&](){
		/* Absolutely Nothing! */
	});

	Tiny::quit();

	return 0;
}

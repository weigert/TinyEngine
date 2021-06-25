#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

int main( int argc, char* args[] ) {

  //Initialize the Compute Shader System
	Tiny::view.vsync = false;
	Tiny::window("Example Window", 800, 800);
  cam::near = -200.0f;
	cam::far = 200.0f;
	cam::zoomrate = 15.0f;
	cam::init(500, cam::ORTHO);

	bool paused = false;
	Tiny::view.interface = [&](){};
  Tiny::event.handler = [&](){

		cam::handler();

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;

	};

  //Get Positions and Velocities
  int nparticles = 4096;
	std::vector<glm::vec4> position;
	std::vector<glm::vec4> velocity;
	std::vector<float> mass;
	for(int i = 0; i < nparticles; i++){
		position.push_back(((glm::vec4(rand()%10000, rand()%10000, rand()%10000, 5000.0f)-5000.0f)/5000.0f));
		velocity.push_back(200.0f*glm::normalize((glm::vec4(rand()%1000, rand()%1000, rand()%1000, 500.0f)-500.0f)/500.0f));
		mass.push_back(1.0f+(float)(rand()%1000)/100.0f);
	}

  //Compute Shader
	Compute compute("shader/gravity.cs", {"position", "velocity", "mass"});
	compute.buffer("position", position);
	compute.buffer("velocity", velocity);
	compute.buffer("mass", mass);

	//Visualization Shader
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Model"});
  Square3D flat;								//Model we want to instance render!
	Instance particle(&flat);			//Particle system based on this model
	std::vector<glm::mat4> models(nparticles, glm::mat4(1));
	particle.addBuffer(models);		//Update particle system

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white
    particleShader.use();
		particleShader.uniform("vp", cam::vp);
		particle.render();

	};

	//Now: Update Positions
	std::vector<int> sorter;
	for(int i = 0; i < nparticles; i++)
		sorter.push_back(i);

	//Execute the render loop
	Tiny::loop([&](){

		if(!paused){

			compute.use();
			compute.uniform("size", nparticles);

			compute.buffer("position", position);
			compute.dispatch(glm::vec3(64/16, 64/16, 1));
			compute.retrieve("position", position);

		}

		//Models, Sorted by View Distance for Correct Alpha Blending
		std::sort(sorter.begin(), sorter.end(), [&](int a, int b){
			float da = glm::length(glm::vec3(position[a])-cam::look+cam::rad*cam::pos);
			float db = glm::length(glm::vec3(position[b])-cam::look+cam::rad*cam::pos);
			return da < db;
		});

		for(int i = 0; i < nparticles; i++){
			glm::mat4 model = cam::invview;
			model = glm::scale(model, glm::vec3(0.001f*mass[sorter[i]]));
			model = glm::translate(glm::mat4(1.0), glm::vec3(position[sorter[i]]))*model;
			models[i] = model;
		}

    particle.updateBuffer(models, 0);

	});

	Tiny::quit();

	return 0;

}

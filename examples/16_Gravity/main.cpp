#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLES 64
#define NPARTICLES (SPARTICLES*SPARTICLES)

int main( int argc, char* args[] ) {

  //Initialize the Compute Shader System
	Tiny::benchmark = true;
	Tiny::view.vsync = false;
	Tiny::window("N-Body Gravity", 1200, 800);
  cam::near = -200.0f;
	cam::far = 200.0f;
	cam::zoomrate = 15.0f;
	cam::init(500, cam::ORTHO);

//	glPointSize(2.0f);

	bool paused = false;
	Tiny::view.interface = [&](){};
  Tiny::event.handler = [&](){

		cam::handler();

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;

	};

  //Get Positions and Velocities
	std::vector<glm::vec4> position;
	std::vector<glm::vec4> velocity;
	std::vector<float> mass;
	for(int i = 0; i < NPARTICLES; i++){
		position.push_back(((glm::vec4(rand()%10000, rand()%10000, rand()%10000, 10000.0f)-5000.0f)/5000.0f));
		velocity.push_back(200.0f*glm::normalize((glm::vec4(rand()%1000, rand()%1000, rand()%1000, 500.0f)-500.0f)/500.0f));
		mass.push_back(1.0f+(float)(rand()%1000)/100.0f);
	}

	//3 Buffers
	Buffer posbuf(position);
	Buffer velbuf(velocity);
	Buffer massbuf(mass);

  //Compute Shader with SSBO Binding Points
	Compute compute("shader/gravity.cs", {"position", "velocity", "mass"});

	//Link Buffers to the SSBO Binding Points
	compute.bind<glm::vec4>("position", &posbuf);
	compute.bind<glm::vec4>("velocity", &velbuf);
	compute.bind<float>("mass", &massbuf);

	//Use the Buffer as an Attribute of a Model VAO
	Model particles({"in_Pos"});
	particles.bind<glm::vec4>("in_Pos", &posbuf);
	particles.SIZE = NPARTICLES;

	//Visualization Shader, does not need attributes
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos"});

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white
    particleShader.use();
		particleShader.uniform("vp", cam::vp);
		particles.render(GL_POINTS);

	};

	//Execute the render loop
	Tiny::loop([&](){

		if(paused) return;

		compute.use();
		compute.uniform("size", NPARTICLES);
		compute.dispatch(1+SPARTICLES/32, 1+SPARTICLES/32);

	});

	Tiny::quit();

	return 0;

}

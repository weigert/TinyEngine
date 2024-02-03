#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLES 64
#define NPARTICLES (SPARTICLES*SPARTICLES)

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::window("N-Body Gravity", 1200, 800);

  Tiny::cam::ortho ortho(Tiny::view.WIDTH, Tiny::view.HEIGHT, -200.0f, 200.0f, 500.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0));
  ortho.update();
  orbit.update();

  Tiny::cam::camera cam(ortho, orbit);

//	glPointSize(2.0f);

	bool paused = true;
	Tiny::view.interface = [&](){};
  Tiny::event.handler = [&](){

		cam.handler();

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;

	};

  //Get Positions and Velocities
	std::vector<glm::vec4> position;
	std::vector<glm::vec4> velocity;
	std::vector<float> mass;
	for(int i = 0; i < NPARTICLES; i++){
		position.push_back(((glm::vec4(rand()%10000, rand()%10000, rand()%10000, 10000.0f)-5000.0f)/5000.0f));
		velocity.emplace_back(0.0, 0.0, 0.0, 0.0);
		mass.push_back(1.0f);
	}

	//3 Buffers
	Tiny::Buffer posbuf(position);
	Tiny::Buffer velbuf(velocity);
	Tiny::Buffer massbuf(mass);

  //Compute Shader with SSBO Binding Points
	Tiny::Compute compute_v("shader/gravity_v.cs", {"position", "velocity", "mass"});
	compute_v.bind("position", &posbuf);
	compute_v.bind("velocity", &velbuf);
	compute_v.bind("mass", &massbuf);

	Tiny::Compute compute_p("shader/gravity_p.cs", {"position", "velocity", "mass"});
	compute_p.bind("position", &posbuf);
	compute_p.bind("velocity", &velbuf);
	compute_p.bind("mass", &massbuf);

	//Use the Buffer as an Attribute of a Model VAO
	Tiny::Model particles({"in_Pos"});
	particles.bind<glm::vec4>("in_Pos", posbuf);

	//Visualization Shader, does not need attributes
	Tiny::Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos"});

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white
    particleShader.use();
		particleShader.uniform("vp", cam.vp());
		particles.render(GL_POINTS, NPARTICLES);

	};

	//Execute the render loop
	Tiny::loop([&](){

		if(paused) return;

		compute_v.use();
		compute_v.uniform("size", NPARTICLES);
		compute_v.dispatch(2, 2);

		compute_p.use();
		compute_p.uniform("size", NPARTICLES);
		compute_p.dispatch(2, 2);

	});

	Tiny::quit();

	return 0;

}

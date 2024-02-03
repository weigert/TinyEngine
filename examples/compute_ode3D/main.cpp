#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLES 384
#define NPARTICLES (SPARTICLES*SPARTICLES)

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::window("3D Quadratic ODE", 1000, 800);
	glPointSize(1.5f);

  Tiny::cam::ortho ortho(Tiny::view.WIDTH, Tiny::view.HEIGHT, -200.0f, 200.0f, 5.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
  ortho.update();
  orbit.update();

  Tiny::cam::camera cam(ortho, orbit);
  cam.update();

	Tiny::view.interface = [&](){};

	bool paused = true;
	bool streak = false;
  Tiny::event.handler = [&](){

		cam.handler();

		if(Tiny::event.active[SDLK_SPACE]) streak = true;
		else streak = false;

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;

	};

	const std::function<float()> randf = [](){
		return (float)(rand()%200000)/100000.0f-1.0f;
	};

	std::vector<glm::vec4> position;
	for(int i = 0; i < NPARTICLES; i++)
		position.push_back(glm::vec4(randf(),randf(),randf(), 1));

	Tiny::Buffer posbuf(position);
	Tiny::Buffer colbuf;
	colbuf.set<glm::vec4>(NPARTICLES, NULL);

  //Compute Shader
	Tiny::Compute compute("shader/ODE.cs", {"position", "color"});
	compute.bind("position", &posbuf);
	compute.bind("color", &colbuf);

	//Visualization Shader
	Tiny::Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos", "in_Col"});

	Tiny::Model particles({"position", "color"});
	particles.bind<glm::vec4>("position", posbuf);
	particles.bind<glm::vec4>("color", colbuf);

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0), !streak);	//Clear Screen to white

		particleShader.use();
		particleShader.uniform("vp", cam.vp());
		particleShader.uniform("op", ((streak)?0.3f:1.0f));
		particles.render(GL_POINTS, NPARTICLES);

	};

	Tiny::loop([&](){

		//cam::pan(-0.2f);

		if(paused) return;

		compute.use();
		compute.uniform("size", NPARTICLES);
		compute.dispatch(SPARTICLES/32, SPARTICLES/32);


	});

	Tiny::quit();

	return 0;

}

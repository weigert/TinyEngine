#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLES 384
#define NPARTICLES (SPARTICLES*SPARTICLES)

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::window("3D Quadratic ODE", 1000, 800);
	glPointSize(1.5f);

  cam::near = -200.0f;
	cam::far = 200.0f;
	cam::zoomrate = 5.0f;
	cam::look = glm::vec3(0, 10, 0);
	cam::init(50, cam::ORTHO);
	cam::rot = -45.0f;
	cam::update();

	Tiny::view.interface = [&](){};

	bool paused = true;
	bool streak = false;
  Tiny::event.handler = [&](){

		cam::handler();

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

	Buffer posbuf(position);
	Buffer colbuf;
	colbuf.fill<glm::vec4>(NPARTICLES, NULL);

  //Compute Shader
	Compute compute("shader/ODE.cs", {"position", "color"});
	compute.bind<glm::vec4>("position", &posbuf);
	compute.bind<glm::vec4>("color", &colbuf);

	//Visualization Shader
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos", "in_Col"});

	Model particles({"position", "color"});
	particles.bind<glm::vec4>("position", &posbuf);
	particles.bind<glm::vec4>("color", &colbuf);
	particles.SIZE = NPARTICLES;

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0), !streak);	//Clear Screen to white

		particleShader.use();
		particleShader.uniform("vp", cam::vp);
		particleShader.uniform("op", ((streak)?0.3f:1.0f));
		particles.render(GL_POINTS);

	};

	Tiny::loop([&](){

		cam::pan(-0.2f);

		if(paused) return;

		compute.use();
		compute.uniform("size", NPARTICLES);
		compute.dispatch(SPARTICLES/32, SPARTICLES/32);


	});

	Tiny::quit();

	return 0;

}

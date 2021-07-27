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

  //Compute Shader
	Compute compute("shader/ODE.cs", {"position", "color"});
	compute.buffer("position", position);										//Buffer Positions into Compute Shader
	compute.buffer("color", (glm::vec4*)NULL, NPARTICLES);	//Empty Color Buffer into Compute Shader

	//Visualization Shader
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos", "in_Col"});

	//Shader SSBO with Particle Shader
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindVertexBuffer(0, Compute::ssbo["position"], 0, sizeof(glm::vec4));

	glEnableVertexAttribArray(1);
	glBindVertexBuffer(1, Compute::ssbo["color"], 0, sizeof(glm::vec4));

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0), !streak);	//Clear Screen to white

		particleShader.use();
		particleShader.uniform("vp", cam::vp);
		particleShader.uniform("op", ((streak)?0.3f:1.0f));

		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, NPARTICLES);

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
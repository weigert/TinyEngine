#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLES 128
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

  //Compute Shader
	Compute compute("shader/gravity.cs", {"position", "velocity", "mass"});
	compute.buffer("position", position);
	compute.buffer("velocity", velocity);
	compute.buffer("mass", mass);

	//Visualization Shader, does not need attributes
	Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos"});

	//Basically: Share the SSBO with the Particle Shader!
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindVertexBuffer(0, Compute::ssbo["position"], 0, sizeof(glm::vec4));

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white
    particleShader.use();
		particleShader.uniform("vp", cam::vp);

		glBindVertexArray(vao);
		glDrawArrays(GL_POINTS, 0, NPARTICLES);

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

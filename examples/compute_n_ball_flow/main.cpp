#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLESX 200
#define SPARTICLESY 100

#define RADIUS 0.004f
#define NPARTICLES (SPARTICLESX*SPARTICLESY)

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::view.antialias = 0;
	Tiny::window("N-Ball Collisions", 800, 400);

  bool paused = true;
	Tiny::event.press[SDLK_p]([&paused](bool pressed){
		if(pressed) paused = !paused;
	});

  //Get Positions and Velocities
	std::vector<glm::vec4> position;
	std::vector<glm::vec4> velocity;
	std::vector<float> mass;

	for(int i = 1; i <= SPARTICLESX; i++){
		for(int j = 1; j <= SPARTICLESY; j++){

			glm::vec4 pos = glm::vec4(2.0f*(float)i/(SPARTICLESX+1)-1.0f, 2.0f*(float)j/(SPARTICLESY+1)-1.0f, 0, 1);
			const glm::vec2 scale = glm::vec2(1, 8.0/4.0);

			if(glm::length(glm::vec2(pos)/scale - glm::vec2(0.5, 0)/scale) < 0.15)
				continue;

      if(glm::length(glm::vec2(pos)/scale - glm::vec2(-0.5, 0)/scale) < 0.15)
				continue;

      if(glm::length(glm::vec2(pos)/scale - glm::vec2(0, 0.5)/scale) < 0.15)
				continue;

      if(glm::length(glm::vec2(pos)/scale - glm::vec2(0, -0.5)/scale) < 0.15)
				continue;

			position.push_back(pos);
			//velocity.push_back(200.0f*glm::normalize((glm::vec4(rand()%1000, rand()%1000, rand()%1000, 500.0f)-500.0f)/500.0f));
			velocity.push_back(glm::vec4(0,0,0,0));

		}
	}

	//3 Buffers
	Tiny::Buffer posbufA(position);
	Tiny::Buffer posbufB(position);
	Tiny::Buffer velbufA(velocity);
	Tiny::Buffer velbufB(velocity);
	Tiny::Buffer massbuf(mass);

  //Compute Shader with SSBO Binding Points
	Tiny::Compute compute("shader/collide.cs");

	Tiny::Square2D flat;
	Tiny::Instance particles(flat);
	particles.bind<glm::vec4>(posbufA);

	//Visualization Shader, does not need attributes
	Tiny::Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Quad", "in_Tex", "in_Pos"});

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white
    particleShader.use();
		particleShader.uniform("R", RADIUS);
		particleShader.storage("velocityA", velbufA);
		particleShader.storage("velocityB", velbufB);
		particles.render(GL_TRIANGLE_STRIP, NPARTICLES);

	};

	//Execute the render loop
	Tiny::loop([&](){

		if(paused) return;

		compute.use();
		compute.uniform("size", (int)NPARTICLES);
		compute.uniform("R", RADIUS);
		compute.storage("positionA", posbufA);
		compute.storage("positionB", posbufB);
		compute.storage("velocityA", velbufA);
		compute.storage("velocityB", velbufB);

		compute.uniform("collide", true);
		compute.dispatch(1+SPARTICLESX/32, 1+SPARTICLESY/32);

		compute.uniform("collide", false);
		compute.dispatch(1+SPARTICLESX/32, 1+SPARTICLESY/32);

	});

	Tiny::quit();

	return 0;

}

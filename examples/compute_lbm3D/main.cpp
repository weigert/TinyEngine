#include <TinyEngine/TinyEngine>
#include <TinyEngine/color>
#include <TinyEngine/camera>

#include <random>

#define GRIDSIZE 64

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::window("Lattice Boltzmann Method 3D, Terrain BC", 800, 800);			//Open Window

	cam::near = -100.0f;
	cam::far = 100.0f;
	cam::rot = 45.0f;
	cam::roty = 45.0f;
	cam::init(10, cam::ORTHO);
	cam::update();

	bool paused = true;

	Tiny::event.handler = [&](){

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;
			
		cam::handler();

	};								//Event Handler
	Tiny::view.interface = [&](){ /* ... */ };				//No Interface

	setup();																					//Prepare Model Stuff

	Tiny::Buffer positions, normals;												//Define Buffers
	Tiny::Buffer indices;
	construct(positions, normals, indices);						//Call algorithm to fill buffers

	Tiny::Indexed mesh({"in_Position", "in_Normal"}, indices);					//Create Model with 2 Properties
	mesh.bind<glm::vec3>("in_Position", positions);	//Bind Buffer to Property
	mesh.bind<glm::vec3>("in_Normal", normals);
  //mesh.set<int>(indices);
  mesh._size = indices.size();

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-GRIDSIZE/2, -15.0, -GRIDSIZE/2));

	Tiny::Shader defaultShader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal"});

	/*
	==============================================================================
												Lattice Boltzmann Method
	==============================================================================
	*/

	// Simulation Scale

	const int NX = 64;
	const int NY = 32;
	const int NZ = 64;
	const int Q = 19;

  // Directions

  glm::vec4* dirs = new glm::vec4[NX*NY*NZ];

  for(size_t x = 0; x < NX; x++)
  for(size_t y = 0; y < NY; y++)
  for(size_t z = 0; z < NZ; z++)
    dirs[(x*NY + y)*NZ + z] = glm::vec4(0);

  Tiny::Buffer dirbuf(NX*NY*NZ, dirs);

  // Lattice Boltzmann Arrays

  Tiny::Buffer f(NX*NY*NZ*Q, (float*)NULL);       //Raw F Buffer
  Tiny::Buffer fprop(NX*NY*NZ*Q, (float*)NULL);   //Raw FProp Buffer (Collision Step)
  Tiny::Buffer rho(NX*NY*NZ, (float*)NULL);       //Density (For Efficiency)

  // Boundary Condition

  float* boundary = new float[NX*NY*NZ];

  for(size_t x = 0; x < NX; x++)
  for(size_t y = 0; y < NY; y++)
  for(size_t z = 0; z < NZ; z++){

	  if(heightmap[x][z] > (float)y/(float)(NY-1))
	    boundary[(x*NY + y)*NZ + z] = 1.0;

    else
      boundary[(x*NY + y)*NZ + z] = 0.0f;

  }

  Tiny::Buffer b(NX*NY*NZ, boundary);

  // Initialization Compute Shader

  Tiny::Compute init("shader/LBM/init.cs", {"f", "fprop", "b", "rho", "v"});
  init.bind("f", &f);
  init.bind("fprop", &fprop);
  init.bind("b", &b);
  init.bind("rho", &rho);
  init.bind("v", &dirbuf);

  init.use();
  init.uniform("NX", NX);
  init.uniform("NY", NY);
  init.uniform("NZ", NZ);
  init.dispatch(NX/16, NY, NZ/16);

  // Collision and Streaming Compute Shaders

  Tiny::Compute collide("shader/LBM/collide.cs", {"f", "fprop", "b", "rho", "v"});
  collide.bind("f", &f);
  collide.bind("fprop", &fprop);
  collide.bind("b", &b);
  collide.bind("rho", &rho);
  collide.bind("v", &dirbuf);

  Tiny::Compute stream("shader/LBM/stream.cs", {"f", "fprop", "b"});
  stream.bind("f", &f);
  stream.bind("fprop", &fprop);
  stream.bind("b", &b);

  /*
  =========================================
              Flow Visualization
  =========================================
  */

  // Particle Positions

  std::vector<glm::vec4> pos;

  std::uniform_real_distribution<double> u(0.0, 1.0);
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator (seed);

  const int NPARTICLE = (2 << 13);

  for(size_t i = 0; i < NPARTICLE; i++){
    glm::vec4 p = glm::vec4(u(generator), u(generator), u(generator), 1)*glm::vec4(NX, NY, NZ, 1);
    if(heightmap[(int)(p.x)][(int)(p.z)] < (float)p.y/(float)(NY-1))
      pos.push_back(p);
  }

  Tiny::Buffer posbuf(pos);

  // Model for Rendering Position, Direction Data

	Tiny::Shader streamshader({"shader/stream.vs", "shader/stream.gs", "shader/stream.fs"}, {"in_Position", "in_Direction"});
  Tiny::Model windmodel({"in_Position", "in_Direction"});
  windmodel.bind<glm::vec4>("in_Position", posbuf);
  windmodel.bind<glm::vec4>("in_Direction", dirbuf);

  // Shader to Move Particles Along

  Tiny::Compute move("shader/move.cs", {"b", "v", "p"});
  move.bind("b", &b);
  move.bind("v", &dirbuf);
  move.bind("p", &posbuf);














	Tiny::view.pipeline = [&](){											//Setup Drawing Pipeline

		Tiny::view.target(color::black);								//Target Screen

		glLineWidth(1.0f);

		defaultShader.use();														//Prepare Shader
		defaultShader.uniform("model", model);			//Set Model Matrix
		defaultShader.uniform("vp", cam::vp);						//View Projection Matrix
		mesh.render(GL_LINES);													//Render Model with Lines

		glLineWidth(2.0f);

		streamshader.use();
		streamshader.uniform("model", model);			//Set Model Matrix
		streamshader.uniform("vp", cam::vp);
		streamshader.uniform("NX", NX);
		streamshader.uniform("NY", NY);
		streamshader.uniform("NZ", NZ);
		windmodel.render(GL_POINTS, windmodel.size());

	};

	float t = 0.0f;

	Tiny::loop([&](){ //Autorotate Camera

		if(paused)
			return;

		t += 0.005f;

    collide.use();
    collide.uniform("NX", NX);
    collide.uniform("NY", NY);
    collide.uniform("NZ", NZ);
    collide.dispatch(NX/16, NY, NZ/16);

	  stream.use();
	  stream.uniform("NX", NX);
	  stream.uniform("NY", NY);
	  stream.uniform("NZ", NZ);
	  stream.uniform("t", t);
	  stream.dispatch(NX/16, NY, NZ/16);

    move.use();
    move.uniform("NX", NX);
    move.uniform("NY", NY);
    move.uniform("NZ", NZ);
    move.dispatch(NPARTICLE/1024);

    // Retrieve the Directions

    posbuf.get(pos);

    for(auto& p: pos){

      if(p.x >= NX-1.0
      || p.y >= NY-1.0
      || p.z >= NZ-1.0
      || p.x <= 0.0
      || p.y <= 0.0
      || p.z <= 0.0
      || heightmap[(int)p.x][(int)p.z] >= (float)p.y/(float)(NY-1)
      || u(generator) > 0.995
      ){
        p = glm::vec4(u(generator), u(generator), u(generator), 1)*glm::vec4(NX, NY, NZ, 1);
        while(heightmap[(int)p.x][(int)p.z] >= (float)p.y/(float)(NY-1))
          p = glm::vec4(u(generator), u(generator), u(generator), 1)*glm::vec4(NX, NY, NZ, 1);
      }

    }

    posbuf.set(pos);

	});

	Tiny::quit();

	return 0;
}

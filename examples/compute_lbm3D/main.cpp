#include <TinyEngine/TinyEngine>
#include <TinyEngine/color>
#include <TinyEngine/camera>

#include <random>

#define GRIDSIZE 64

#include "model.h"

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::window("Lattice Boltzmann Method 3D, Terrain BC", 800, 800);			//Open Window

  Tiny::cam::orthogonal ortho({Tiny::view.WIDTH, Tiny::view.HEIGHT}, {-100.0f, 100.0f}, 10.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
  Tiny::camera cam(ortho, orbit);
  cam.update();

  bool paused = true;
	Tiny::event.press[SDLK_p]([&paused](bool t){
		paused = !paused;
	});

	Tiny::event.handler = cam.handler;
	Tiny::view.interface = [&](){ /* ... */ };				//No Interface

	setup();																					//Prepare Model Stuff

	Tiny::Buffer positions, normals;												//Define Buffers
	Tiny::Buffer indices;
	construct(positions, normals, indices);						//Call algorithm to fill buffers

	Tiny::Indexed mesh({"in_Position", "in_Normal"}, indices);					//Create Model with 2 Properties
	mesh.bind<glm::vec3>("in_Position", positions);	//Bind Buffer to Property
	mesh.bind<glm::vec3>("in_Normal", normals);

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

  Tiny::Compute init("shader/LBM/init.cs");

  init.use();
  init.uniform("NX", NX);
  init.uniform("NY", NY);
  init.uniform("NZ", NZ);
  init.storage("f", f);
  init.storage("fprop", fprop);
  init.storage("b", b);
  init.storage("rho", rho);
  init.storage("v", dirbuf);
  init.dispatch(NX/16, NY, NZ/16);

  // Collision and Streaming Compute Shaders

  Tiny::Compute collide("shader/LBM/collide.cs");
  Tiny::Compute stream("shader/LBM/stream.cs");

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

  Tiny::Compute move("shader/move.cs");

	Tiny::view.pipeline = [&](){											//Setup Drawing Pipeline

		Tiny::view.target(color::black);								//Target Screen

		glLineWidth(1.0f);

		defaultShader.use();														//Prepare Shader
		defaultShader.uniform("model", model);			//Set Model Matrix
		defaultShader.uniform("vp", cam.vp());						//View Projection Matrix
		mesh.render(GL_LINES);													//Render Model with Lines

		glLineWidth(2.0f);

		streamshader.use();
		streamshader.uniform("model", model);			//Set Model Matrix
		streamshader.uniform("vp", cam.vp());
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
    collide.storage("f", f);
    collide.storage("fprop", fprop);
    collide.storage("b", b);
    collide.storage("rho", rho);
    collide.storage("v", dirbuf);
    collide.dispatch(NX/16, NY, NZ/16);

	  stream.use();
	  stream.uniform("NX", NX);
	  stream.uniform("NY", NY);
	  stream.uniform("NZ", NZ);
	  stream.uniform("t", t);
    stream.storage("f", f);
    stream.storage("fprop", fprop);
    stream.storage("b", b);
	  stream.dispatch(NX/16, NY, NZ/16);

    move.use();
    move.uniform("NX", NX);
    move.uniform("NY", NY);
    move.uniform("NZ", NZ);
    move.storage("b", b);
    move.storage("v", dirbuf);
    move.storage("p", posbuf);
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

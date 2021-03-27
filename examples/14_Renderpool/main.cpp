#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include "renderpool.cpp"
#include "chunk.h"

using namespace glm;

int main( int argc, char* args[] ) {

	srand(0);

	Tiny::view.vsync = false;
	Tiny::benchmark = true;
	Tiny::window("Example Window", 600, 400);

  cam::near = -500.0f;
  cam::far = 500.0f;
  cam::rad = 1.0f;
  cam::look = vec3(32,0,32);
  cam::init(5, cam::ORTHO);

	const int N = 10*5*10*6;	//6 Face Orientations per Chunk

	Renderpool<Vertex> vertpool(3000*N, N);

	vector<Chunk> chunks;
	Chunk chunk;

	for(int i = 0; i < 10; i++)
	for(int j = 0; j < 5; j++)
	for(int k = 0; k < 10; k++){

		chunk.randomize();
		chunk.pos = ivec3(i,j,k);
		chunks.push_back(chunk);

		chunkmesh::greedypool(&chunks.back(), &vertpool, (k+j*10+i*10*5));

	}

	//Render Pass

	//Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});
	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Color"});

  glm::vec3 previouspos = cam::pos;
	bool docull = false;

	Tiny::event.handler = [&](){ cam::handler();

		if(cam::moved && docull){

			if(cam::pos.x*previouspos.x <= 1E-6){

				std::cout<<"Cull Switch"<<std::endl;

				for(int i = 0; i < chunks.size(); i++){

					if(cam::pos.x < 0) vertpool.activate(chunks[i].faces[0]);
					if(cam::pos.x > 0) vertpool.deactivate(chunks[i].faces[0]);
					if(cam::pos.x > 0) vertpool.activate(chunks[i].faces[1]);
					if(cam::pos.x < 0) vertpool.deactivate(chunks[i].faces[1]);

				}

			}

			if(cam::pos.y*previouspos.y <= 1E-6){

				std::cout<<"Cull Switch"<<std::endl;

				for(int i = 0; i < chunks.size(); i++){

					if(cam::pos.y < 0) vertpool.activate(chunks[i].faces[2]);
					if(cam::pos.y > 0) vertpool.deactivate(chunks[i].faces[2]);
					if(cam::pos.y > 0) vertpool.activate(chunks[i].faces[3]);
					if(cam::pos.y < 0) vertpool.deactivate(chunks[i].faces[3]);

				}

			}

			if(cam::pos.z*previouspos.z <= 1E-6){

				std::cout<<"Cull Switch"<<std::endl;

				for(int i = 0; i < chunks.size(); i++){

					if(cam::pos.z < 0) vertpool.activate(chunks[i].faces[4]);
					if(cam::pos.z > 0) vertpool.deactivate(chunks[i].faces[4]);
					if(cam::pos.z > 0) vertpool.activate(chunks[i].faces[5]);
					if(cam::pos.z < 0) vertpool.deactivate(chunks[i].faces[5]);

				}

			}

			previouspos = cam::pos;

		}

	};

	Tiny::view.interface = [&](){ /* ... */ };

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white
		shader.use();
		shader.uniform("vp", cam::vp);


		if(cam::pos.x < 0) vertpool.render(0*N/6, N/6);
		else if(cam::pos.x > 0) vertpool.render(1*N/6, N/6);
		if(cam::pos.y < 0) vertpool.render(2*N/6, N/6);
		else if(cam::pos.y > 0) vertpool.render(3*N/6, N/6);
		if(cam::pos.z < 0) vertpool.render(4*N/6, N/6);
		else if(cam::pos.z > 0) vertpool.render(5*N/6, N/6);

//		vertpool.render();

	};

	Tiny::loop([&](){ /* ... */
		std::cout<<Tiny::average<<std::endl;
	});
	Tiny::quit();

	return 0;

}

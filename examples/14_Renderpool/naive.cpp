#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include "renderpool.cpp"
#include "chunk.h"

using namespace glm;

int main( int argc, char* args[] ) {

	srand(0);

	Tiny::view.vsync = false;
	Tiny::benchmark = true;
	Tiny::window("Example Window", 1200, 800);

	cam::near = -500.0f;
  cam::far = 500.0f;
	cam::rot = 45.0f;
	cam::roty = 45.0f;
  cam::look = vec3(32, 0, 32);
  cam::init(3.5, cam::ORTHO);
	cam::update();

	Chunk::LOD = 1;
	Chunk::QUAD = 3500;
	//Chunk::LOD = 2;
	//Chunk::QUAD = 1800;
	//Chunk::LOD = 4;
	//Chunk::QUAD = 200;
	//Chunk::LOD = 8;
	//Chunk::QUAD = 100;

  //Naive Approach

	vector<Model*> models;
	vector<Chunk> chunks;
	Chunk chunk;

	std::cout<<"Meshing ";
	timer::benchmark<std::chrono::microseconds>([&](){

  for(int i = 0; i < 5; i++)
  for(int j = 0; j < 5; j++)
  for(int k = 0; k < 5; k++){

		chunk.randomize();
    chunk.pos = ivec3(i, j, k);

    Model* model = new Model(chunkmesh::greedy, &chunk);
    models.push_back(model);
		chunks.push_back(chunk);

  }

	});

	Shader shader({"shader/naive.vs", "shader/naive.fs"}, {"in_Position", "in_Normal", "in_Color"});

	Tiny::event.handler = [&](){ cam::handler(); };
	Tiny::view.interface = [&](){ /* ... */ };

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white
		shader.use();
    shader.uniform("vp",  cam::vp);
    for(int i = 0; i < models.size(); i++)
      models[i]->render();

	};

	Tiny::loop([&](){ /* ... */

		if(Tiny::benchmark) std::cout<<Tiny::average<<std::endl;

		int r;
	//	int t = 0;

		for(int i = 0 ; i < 50; i++){

			r = rand()%chunks.size();
			chunks[r].randomize();

		//	std::cout<<"Chunk Edit ";
		//	t += timer::benchmark<std::chrono::microseconds>([&](){

			models[r]->construct(chunkmesh::greedy, &chunks[r]);

		//	});

		}

		// std::cout<<t<<std::endl;

		if(models.size() == 0) Tiny::event.quit = true;

	});

	delete[] chunk.data;
	Tiny::quit();
	return 0;

}

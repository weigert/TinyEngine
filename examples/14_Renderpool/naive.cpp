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
  cam::rad = 1.0f;
  cam::look = vec3(32,0,32);
  cam::init(5, cam::ORTHO);

	Chunk::LOD = 4;
	Chunk::QUAD = 200;

  //Naive Approach

  vector<Model*> models;
	Chunk chunk;

	std::cout<<"Meshing ";
	timer::benchmark<std::chrono::microseconds>([&](){

  for(int i = 0; i < 15; i++)
  for(int j = 0; j < 15; j++)
  for(int k = 0; k < 15; k++){

		chunk.randomize();
    chunk.pos = ivec3(i, j, k);

    Model* model = new Model(chunkmesh::greedy, &chunk);
    models.push_back(model);
	//	models.back()->indexed = false;

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
		std::cout<<Tiny::average<<std::endl;

		int r = rand()%models.size();

		chunk.randomize();
		chunk.pos = models[r]->pos;

		std::cout<<"Chunk Edit ";
		timer::benchmark<std::chrono::microseconds>([&](){

		models[r]->construct(chunkmesh::greedy, &chunk);

		});

		if(models.size() == 0) Tiny::event.quit = true;

	});
	Tiny::quit();

	return 0;

}

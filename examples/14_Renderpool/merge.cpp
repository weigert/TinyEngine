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

  //Naive Approach

  vector<Chunk> chunks;
  vector<Model*> models;

	Chunk chunk;
	Model fullmodel;
	fullmodel.indexed = false;
	Model singlemodel;

  for(int i = 0; i < 10; i++)
  for(int j = 0; j < 5; j++)
  for(int k = 0; k < 10; k++){

		chunk.randomize();
    chunk.pos = 2*ivec3(i, j, k);

		/*
		    Model* model = new Model(chunkmesh::greedy, &chunk);
		    model->indexed = false;
		    models.push_back(model);
		*/

  	singlemodel.positions.clear();
		singlemodel.normals.clear();
		singlemodel.colors.clear();

		chunkmesh::greedy(&singlemodel, &chunk);

		fullmodel.positions.insert(fullmodel.positions.end(), singlemodel.positions.begin(), singlemodel.positions.end());
		fullmodel.colors.insert(fullmodel.colors.end(), singlemodel.colors.begin(), singlemodel.colors.end());
	  fullmodel.normals.insert(fullmodel.normals.end(), singlemodel.normals.begin(), singlemodel.normals.end());

		std::cout<<i<<" "<<j<<" "<<k<<std::endl;
  }

	fullmodel.update();

	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});

	Tiny::event.handler = [&](){ cam::handler(); };
	Tiny::view.interface = [&](){ /* ... */ };

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white
		shader.use();
    shader.uniform("vp",  cam::vp);
		fullmodel.render();
    //for(int i = 0; i < models.size(); i++)
    //  models[i]->render();

	};

	Tiny::loop([&](){ /* ... */
		std::cout<<Tiny::average<<std::endl;
	});
	Tiny::quit();

	return 0;

}

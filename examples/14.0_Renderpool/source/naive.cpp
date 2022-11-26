#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include "vertexpool.cpp"
#include "chunk.h"

using namespace glm;

int main( int argc, char* args[] ) {

	srand(time(NULL));

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

	std::cout<<"Meshing ";
	timer::benchmark<std::chrono::microseconds>([&](){

  for(int i = 0; i < 5; i++)
  for(int j = 0; j < 5; j++)
  for(int k = 0; k < 5; k++){

		chunks.emplace_back(ivec3(i, j, k));

		//Define Buffers
		Buffer* positions = new Buffer();
		Buffer* normals = new Buffer();
		Buffer* colors = new Buffer();
		Buffer* indices = new Buffer();

		chunkmesh::greedy(&chunks.back(), indices, positions, normals, colors);

		Model* model = new Model({"in_Position", "in_Normal", "in_Color"});
		model->bind<glm::vec3>("in_Position", positions, true);
		model->bind<glm::vec3>("in_Normal", normals, true);
		model->bind<glm::vec4>("in_Color", colors, true);
		model->index(indices, true);

    models.push_back(model);

  }

	});

	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});

	Tiny::event.handler = [&](){ cam::handler(); };
	Tiny::view.interface = [&](){ /* ... */ };

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white
		shader.use();
    shader.uniform("vp",  cam::vp);
    for(int i = 0; i < models.size(); i++)
      models[i]->render(GL_TRIANGLES);

	};


	float at = 0;

	Tiny::loop([&](){ /* ... */


		if(Tiny::benchmark) std::cout<<Tiny::average<<std::endl;

		int r;

		int t = 0;

		for(int i = 0 ; i < chunks.size(); i++){

		//	r = rand()%chunks.size();
			chunks[i].update();

			t += timer::benchmark<std::chrono::microseconds>([&](){

			chunkmesh::greedy(&chunks[i], models[i]->buffers["index"], models[i]->buffers["in_Position"], models[i]->buffers["in_Normal"], models[i]->buffers["in_Color"]);
			models[i]->SIZE = models[i]->buffers["index"]->SIZE;

			}, false);

		}



		 std::cout<<"AVERAGE TIME: "<<t/chunks.size()<<std::endl;
		 at = 0.99f*at + 0.01f*float(t);

		 std::cout<<"AVERAGE ROLL: "<<at/chunks.size()<<std::endl;

	});

	for(auto& chunk: chunks)
		delete[] chunk.data;

	Tiny::quit();
	return 0;

}

#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

#include "renderpool.cpp"
#include "chunk.h"

using namespace glm;

int main( int argc, char* args[] ) {

	srand(time(NULL));

	Tiny::view.vsync = false;
	Tiny::benchmark = true;
	Tiny::window("Example Window", 1200, 800);

//	glEnable(GL_DEPTH_TEST);

  cam::near = -500.0f;
  cam::far = 500.0f;
  cam::rad = 1.0f;
  cam::look = vec3(32, 0, 32);
  cam::init(5, cam::ORTHO);

	const int N = 10*10*10*6;	//6 Face Orientations per Chunk

	//Chunk::LOD = 1;
	//Chunk::QUAD = 3600;

	//	Chunk::LOD = 2;
	//	Chunk::QUAD = 800;
	Chunk::LOD = 4;
	Chunk::QUAD = 200;
	//Chunk::LOD = 8;
	//Chunk::QUAD = 50;

	std::unordered_set<int> groups;

	int K = Chunk::QUAD;

	std::cout<<"SIZE: "<<sizeof(Vertex)<<std::endl;

	Renderpool<Vertex> vertpool(K, N);

	vector<Chunk> chunks;
	Chunk chunk;

	std::cout<<"Meshing ";
	timer::benchmark<std::chrono::microseconds>([&](){

	for(int i = 0; i < 10; i++)
	for(int j = 0; j < 10; j++)
	for(int k = 0; k < 10; k++){

		chunk.randomize();
		chunk.pos = 2*ivec3(i,j,k);
		chunks.push_back(chunk);

		chunkmesh::greedypool(&chunks.back(), &vertpool);

	}

	groups.clear();
	if(cam::pos.x < 0) groups.insert(0);
	if(cam::pos.x > 0) groups.insert(1);
	if(cam::pos.y < 0) groups.insert(2);
	if(cam::pos.y > 0) groups.insert(3);
	if(cam::pos.z < 0) groups.insert(4);
	if(cam::pos.z > 0) groups.insert(5);
	vertpool.mask([&](DAIC& cmd){
		return groups.contains(cmd.group);
	});

/*
	vertpool.mask(groups);
	*/

});




	//Render Pass

	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});

	vec3 oldpos;
	vec3 newpos;

	oldpos.x = (cam::pos.x > 0)?1:-1;
	oldpos.y = (cam::pos.y > 0)?1:-1;
	oldpos.z = (cam::pos.z > 0)?1:-1;
	if(cam::pos.x == 0) oldpos.x = 0;
	if(cam::pos.y == 0) oldpos.y = 0;
	if(cam::pos.z == 0) oldpos.z = 0;

	Tiny::event.handler = [&](){ cam::handler();

		if(cam::moved){

			newpos.x = (cam::pos.x > 0)?1:-1;
			newpos.y = (cam::pos.y > 0)?1:-1;
			newpos.z = (cam::pos.z > 0)?1:-1;
			if(cam::pos.x == 0) newpos.x = 0;
			if(cam::pos.y == 0) newpos.y = 0;
			if(cam::pos.z == 0) newpos.z = 0;

			if(!all(equal(oldpos, newpos))){

				groups.clear();
				if(cam::pos.x < 0) groups.insert(0);
				if(cam::pos.x > 0) groups.insert(1);
				if(cam::pos.y < 0) groups.insert(2);
				if(cam::pos.y > 0) groups.insert(3);
				if(cam::pos.z < 0) groups.insert(4);
				if(cam::pos.z > 0) groups.insert(5);
				vertpool.mask([&](DAIC& cmd){
					return groups.contains(cmd.group);
				});
			}

			oldpos = newpos;

		}

	};

	Tiny::view.interface = [&](){ /* ... */ };

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white
		shader.use();
		shader.uniform("vp", cam::vp);

		vertpool.render();

	};

	Tiny::loop([&](){ /* ... */

		if(Tiny::benchmark) std::cout<<Tiny::average<<std::endl;


		int r;

		std::cout<<"Chunk Edit ";
		timer::benchmark<std::chrono::microseconds>([&](){

		for(int i = 0 ; i < 50; i++){

			r = rand()%chunks.size();

			vertpool.unsection(r*6, 6);

			chunk.randomize();
			chunk.pos = chunks[r].pos;

			chunks.erase(chunks.begin()+r);
			chunks.push_back(chunk);

			chunkmesh::greedypool(&chunks.back(), &vertpool);

		}

		});

		vertpool.mask([&](DAIC& cmd){
			return groups.contains(cmd.group);
		});

		if(chunks.size() == 0) Tiny::event.quit = true;

	});

	Tiny::quit();

	return 0;

}

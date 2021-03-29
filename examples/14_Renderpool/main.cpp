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

  cam::near = -500.0f;
  cam::far = 500.0f;
  cam::rad = 1.0f;
  cam::look = vec3(32,0,32);
  cam::init(5, cam::ORTHO);

	const int N = 5*5*5*6;	//6 Face Orientations per Chunk

	Renderpool<Vertex> vertpool(3000*2*N);

	vector<Chunk> chunks;
	Chunk chunk;

	for(int i = 0; i < 5; i++)
	for(int j = 0; j < 5; j++)
	for(int k = 0; k < 5; k++){

		chunk.randomize();
		chunk.pos = 2*ivec3(i,j,k);
		chunks.push_back(chunk);

		chunkmesh::greedypool(&chunks.back(), &vertpool);

	}

	//Render Pass

	Shader shader({"shader/default.vs", "shader/default.fs"}, {"in_Position", "in_Normal", "in_Color"});

	Tiny::event.handler = [&](){ cam::handler();

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_SPACE){
			std::sort(vertpool.free.begin(), vertpool.free.end());
	//		vertpool.free.sort();
			vertpool.computefracture();
		}

	};

	Tiny::view.interface = [&](){ /* ... */ };

/*
	//Betteer Activation / Deactivation
	vertpool.deactivate(0, 6);
	vertpool.deactivate(2, 6);
	vertpool.deactivate(4, 6);
*/

	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white
		shader.use();
		shader.uniform("vp", cam::vp);

		vertpool.render();

	};

	Tiny::loop([&](){ /* ... */

		if(Tiny::benchmark) std::cout<<Tiny::average<<std::endl;

		int r = rand()%chunks.size();
		vertpool.unsection(r*6, 6);

		chunk.randomize();
		chunk.pos = chunks[r].pos;

		chunks.erase(chunks.begin()+r);
		chunks.push_back(chunk);

		chunkmesh::greedypool(&chunks.back(), &vertpool);

		if(chunks.size() == 0) Tiny::event.quit = true;

	});

	Tiny::quit();

	return 0;

}

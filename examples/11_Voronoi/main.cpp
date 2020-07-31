#include "../../TinyEngine.h"
#include "../../include/helpers/color.h"

#include "timer.h"
#include "poisson.h"

#include "instance.cpp"

glm::vec3 pickvec(int i, int N);
int getpick(glm::vec3 a, int N);

int main( int argc, char* args[] ) {

	Tiny::view.vsync = false;
	Tiny::window("GPU Accelerated Voronoise", 1000, 1000);

	Tiny::event.handler  = [](){}; //eventHandler;
	Tiny::view.interface = [](){}; //interfaceFunc;

	/*

	~ GPU Accelerated Voronoi Map ~

	Possible OpenGL Implementations (2 Types):

		(A) Naive Parallel On-Screen Clustering:
			-> Pass all centroids as uniform, cluster in shader

		(B) Depth Test Clustering:
			-> Instanced render with centroids, distance is depth, color computed in shader

		Here I implemented the second version.

		Additional Optimizations:
			-> Don't draw entire quad when points are placed using poisson disc, only a small quad
				This reduces fragment waste because we can assume cones of a certain size.
				2-3 Orders of Magnitude Speed Boost
			-> Don't pass a matrix, just the centroid. Scale in the vertex shader.
	*/

	//Generate Centroids (Noise)
	std::vector<glm::vec2> centroids;
	srand(time(NULL));
	float K = 8*1024;
	sample::disc(centroids, K, glm::vec2(-1), glm::vec2(1));

	std::vector<glm::mat4> models;
  float R = 2.0f*sqrt(4.0f/3.14159265f/K);
	for(auto& c: centroids){
		glm::mat4 model = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(c.x, c.y, 0)), glm::vec3(R));
		models.push_back(model);
	}

	//Compute Color Hashing Number
	int NCOLOR = 1;
		while(pow(NCOLOR, 3) < centroids.size())
			NCOLOR++;

	//Utility Classes
	Square3D flat;
	Shader voronoi({"shader/voronoi.vs", "shader/voronoi.fs"}, {"in_Quad", "in_Tex", "in_Model", "in_Centroid"});

	Billboard billboard(1000, 1000);
	Shader billboardshader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});
	billboard.target(color::black); //Clear Buffers Once

	Instance instance(&flat);
	instance.addBuffer(models);

	//Make sure we don't over do it
	bool updated = true;

	Tiny::view.pipeline = [&](){

		if(updated){

				billboard.target(color::black);
				voronoi.use();		//Instanced Render Pass
				voronoi.uniform("NCOLOR", NCOLOR);
				instance.render();

				glFlush();

		//	updated = false;
		}

		Tiny::view.target(color::black);	//Target Screen

		billboardshader.use();
		billboardshader.texture("imageTexture", billboard.texture);
		billboardshader.uniform("model", flat.model);
		flat.render();

	};

	Tiny::loop([&](){

	//	std::cout<<"Move Centroids Time"<<std::endl;
	//	timer::benchmark<std::chrono::microseconds>([&](){

/*
		//Move Centroids Around
		for(auto&c: centroids){
			c.x += (rand()%1000-500)/100000.0f;
			c.y += (rand()%1000-500)/100000.0f;
			if(c.x > 1.0) c.x = -1.0;
			if(c.y > 1.0) c.y = -1.0;
		}

		instance.updateBuffer(centroids, 0);
*/
		updated = true;

	//	});


	});

	Tiny::quit();

	return 0;
}

/*
//Color Conversion
glm::vec3 pickvec(int i, int N){

  int Z = (float)(i%N);
  int Y = (float)((i/N)%N);
  int X = (float)((i/(N*N))%N);

  return glm::vec3(X, Y, Z)/glm::vec3(N-1.0f);
}

int getpick(glm::vec3 a, int N){
  glm::vec3 b = glm::vec3(N-1.0f)*glm::vec3(N*N, N, 1);
  return a.x*b.x + a.y*b.y + a.z*b.z;
}
*/

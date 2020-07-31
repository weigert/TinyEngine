#include "../../TinyEngine.h"
#include "../../include/helpers/color.h"

#include "poisson.h"
//#include "model.h"
#include <noise/noise.h>

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
		-> Render to texture, then render quad to screen

	Applications:
		-> Dynamic Run-Time Mosaic Filters in a Shader
		-> Dynamic Run-Time Bubble Filter
		-> Dynamic Run-Time Mosaic Effects with Particles (e.g. Lava-, Cloud-Flow)
		-> Voronoi Painter using Directional Poisson Disc Sampling
		-> "Clustered Convection" by Centroid (Particle) Motion
		-> ???

	Extensions:
		-> Rendering to Sphere using Cubemaps (trivial to extend)
*/

int main( int argc, char* args[] ) {

	//Setup Window
	Tiny::view.vsync = true;
	Tiny::window("GPU Accelerated Voronoise", 1024, 1024);

	Tiny::event.handler  = [](){}; //eventHandler;
	Tiny::view.interface = [](){}; //interfaceFunc;

	srand(time(NULL));

	//Generate Set of Centroids
	std::vector<glm::vec2> centroids;
	std::vector<glm::vec2> offset;
	float K = 1024; //That's a lot of polygons
	sample::disc(centroids, K, glm::vec2(-1), glm::vec2(1));
	offset = centroids;
  float R = 2.0f*sqrt(4.0f/3.14159265f/K);

	//Compute Color Hashing Number
	int NCOLOR = 1;
		while(pow(NCOLOR, 3) < centroids.size())
			NCOLOR++;

	//Utility Classes
	Square2D flat;
	Shader voronoi({"shader/voronoi.vs", "shader/voronoi.fs"}, {"in_Quad", "in_Tex", "in_Centroid"});

	Billboard billboard(1024, 1024);
	Shader billboardshader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});

	Instance instance(&flat);
	instance.addBuffer(centroids);

	noise::module::Perlin perlin;
  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.5);

	Tiny::view.pipeline = [&](){

		billboard.target(color::black);
		voronoi.use();
		voronoi.uniform("NCOLOR", NCOLOR);
		voronoi.uniform("R", R);
		instance.render();

		Tiny::view.target(color::black);	//Target Screen

		billboardshader.use();
		billboardshader.texture("imageTexture", billboard.texture);
		billboardshader.uniform("model", flat.model);
		flat.render();

	};

	float t = 0; //Time

	Tiny::loop([&](){

		t += 0.01;

		//Jiggle Centroids using Continuous Noise
		for(unsigned int i = 0; i < centroids.size(); i++){
			offset[i].x = centroids[i].x + 0.5f*R*perlin.GetValue(centroids[i].x, centroids[i].y, t);
			offset[i].y = centroids[i].y + 0.5f*R*perlin.GetValue(centroids[i].x, centroids[i].y, -t);
		}

		instance.updateBuffer(offset, 0);

	});

	Tiny::quit();

	return 0;
}

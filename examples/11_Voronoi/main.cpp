#include "../../TinyEngine.h"
#include "../../include/helpers/color.h"
#include "../../include/helpers/image.h"

#include "poisson.h"
#include "model.h"
#include <noise/noise.h>
#include <chrono>

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

#define SIZE 768

int main( int argc, char* args[] ) {

	//Setup Window
	Tiny::view.vsync = false;
	Tiny::window("GPU Accelerated Voronoise", SIZE, SIZE);

	Tiny::event.handler  = [](){}; //eventHandler;
	Tiny::view.interface = interfaceFunc;

	srand(time(NULL));

	//Generate Set of Centroids
	sample::disc(centroids, K, glm::vec2(-1), glm::vec2(1));
	offset = centroids;

	//Utility Classes
	Square2D flat;
	Shader voronoi({"shader/voronoi.vs", "shader/voronoi.fs"}, {"in_Quad", "in_Tex", "in_Centroid"});

	Billboard billboard(SIZE, SIZE);
	Shader billboardshader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});

	//Filter Effects
	Shader bubble({"shader/bubble.vs", "shader/bubble.fs"}, {"in_Quad", "in_Tex", "in_Centroid"}, {"centroids"});
	Shader mosaic({"shader/mosaic.vs", "shader/mosaic.fs"}, {"in_Quad", "in_Tex", "in_Centroid"}, {"centroids"});

	bubble.buffer("centroids", centroids);
	mosaic.buffer("centroids", centroids);

	Instance instance(&flat);
	instance.addBuffer(centroids);

	Texture tex(image::load("starry_night.png"));		//Load Texture with Image

	noise::module::Perlin perlin;
  perlin.SetOctaveCount(4);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.5);

	int n = 0;
	float us = 0.0;

	Tiny::view.pipeline = [&](){

		auto start = std::chrono::high_resolution_clock::now();

		billboard.target(color::black);
		voronoi.use();
		voronoi.uniform("R", R);
		voronoi.uniform("drawcenter", drawcenter);
		voronoi.uniform("style", drawstyle);
		voronoi.uniform("metric", metric);
		voronoi.uniform("twist", twist);
		instance.render();

		glFlush();

		auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

		//Rolling Average
		us = (float)((us*n+duration.count())/(n+1));
		n++;

		std::cout<<"Average Execution Time: "<<us<<std::endl;

		Tiny::view.target(color::black);	//Target Screen

		if(drawstyle == 3){
			bubble.use();
			bubble.texture("voronoi", billboard.texture);
			bubble.texture("image", tex);
			bubble.uniform("R", R);
			bubble.uniform("model", flat.model);
			flat.render();
		}

		else if(drawstyle == 2) {
			mosaic.use();
			mosaic.texture("voronoi", billboard.texture);
			mosaic.texture("image", tex);
			mosaic.uniform("R", R);
			mosaic.uniform("model", flat.model);
			flat.render();
		}

		else {
			billboardshader.use();
			billboardshader.texture("imageTexture", billboard.texture);
			billboardshader.uniform("model", flat.model);
			flat.render();
		}
	};

	float t = 0; //Time

	Tiny::loop([&](){

		if(animate){

			t += 0.005;

			//Jiggle Centroids using Continuous Noise
			for(unsigned int i = 0; i < centroids.size(); i++){
				offset[i].x = centroids[i].x + 0.5f*R*perlin.GetValue(centroids[i].x, centroids[i].y, t);
				offset[i].y = centroids[i].y + 0.5f*R*perlin.GetValue(centroids[i].x, centroids[i].y, -t);
			}

		instance.updateBuffer(offset, 0); //Move Things as Animated!

		}

		if(translate){ //Update Bubble Centroids (if desired and applicable)
			if(drawstyle == 2) mosaic.buffer("centroids", offset);
			if(drawstyle == 3) bubble.buffer("centroids", offset);
		}

		if(updated){ //Update the data in the buffers!
			bubble.buffer("centroids", centroids);
			mosaic.buffer("centroids", centroids);
			instance.updateBuffer(offset, 0); //Move Things as Animated!
			updated = false;
		}

	});

	Tiny::quit();

	return 0;
}

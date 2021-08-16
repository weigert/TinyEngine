#include <TinyEngine/TinyEngine>
#include <TinyEngine/color>
#include <TinyEngine/image>

#include "poisson.h"
#include "model.h"
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

#define _SIZE_ 768

int main( int argc, char* args[] ) {

	//Setup Window
	Tiny::view.vsync = false;
	Tiny::window("GPU Accelerated Voronoise", _SIZE_, _SIZE_);

	Tiny::event.handler  = [](){}; //eventHandler;
	Tiny::view.interface = interfaceFunc;

	srand(time(NULL));

	//Generate Set of Centroids
	sample::disc(centroids, K, glm::vec2(-1), glm::vec2(1));
	offset = centroids;
	Buffer centroidbuf(centroids);

	//Utility Classes
	Square2D flat;
	Shader voronoi({"shader/voronoi.vs", "shader/voronoi.fs"}, {"in_Quad", "in_Tex", "in_Centroid"});

	Billboard billboard(_SIZE_, _SIZE_);
	Shader billboardshader({"shader/billboard.vs", "shader/billboard.fs"}, {"in_Quad", "in_Tex"});

	//Filter Effects
	Shader bubble({"shader/bubble.vs", "shader/bubble.fs"}, {"in_Quad", "in_Tex", "in_Centroid"}, {"centroids"});
	Shader mosaic({"shader/mosaic.vs", "shader/mosaic.fs"}, {"in_Quad", "in_Tex", "in_Centroid"}, {"centroids"});

	//SSBO Centroids into Filters
	bubble.bind<glm::vec2>("centroids", &centroidbuf);
	mosaic.bind<glm::vec2>("centroids", &centroidbuf);

	//Prepare instance render of flat, per-centroid
	Instance instance(&flat);
	instance.bind<glm::vec2>("in_Centroid", &centroidbuf);

	Texture tex(image::load("starry_night.png")); //Load Texture with Image

	//Prepare Noise for jiggling the centroids
	FastNoiseLite noise;
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalOctaves(8.0f);
  noise.SetFractalLacunarity(2.0f);
  noise.SetFractalGain(0.6f);
  noise.SetFrequency(1.0);

	int n = 0;
	float us = 0.0; //Rolling average execution time calculation in microseconds (us)

	Tiny::view.pipeline = [&](){

		auto start = std::chrono::high_resolution_clock::now(); //Start Timer

		billboard.target(color::black);
		voronoi.use();
		voronoi.uniform("R", R);
		voronoi.uniform("drawcenter", drawcenter);
		voronoi.uniform("style", drawstyle);
		voronoi.uniform("metric", metric);
		voronoi.uniform("twist", twist);
		instance.render();

		glFlush();						//Finish Pass

		auto stop = std::chrono::high_resolution_clock::now();	//TIME!
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

		//Rolling Average
		us = (float)((us*n+duration.count())/(n+1));
		n++;

		std::cout<<"Average Execution Time: "<<us<<std::endl; 	//Comment out for less spam

		Tiny::view.target(color::black);	//Target Screen

		//Pick a shader, pass textures, render as a quad.
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

	Tiny::loop([&](){ //Execute every frame

		if(animate){

			t += 0.005;

			//Jiggle Centroids using Continuous Noise
			for(unsigned int i = 0; i < centroids.size(); i++){
				offset[i].x = centroids[i].x + 0.5f*R*noise.GetNoise(centroids[i].x, centroids[i].y, t);
				offset[i].y = centroids[i].y + 0.5f*R*noise.GetNoise(centroids[i].x, centroids[i].y, -t);
			}

			//Centroids have moved, so we update the instance buffer
			centroidbuf.fill<glm::vec2>(offset);

		}

		if(updated){ //Update the data in the buffers!
			centroidbuf.fill<glm::vec2>(centroids);
			instance.SIZE = centroids.size();
			updated = false;
		}

	});

	Tiny::quit();

	return 0;
}

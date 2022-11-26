#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <random>

#include "icosphere.h"
#include "FastNoiseLite.h"

std::uniform_real_distribution<double> u(0.0, 1.0);

glm::vec3 spheresample(std::mt19937& g){

  double theta = 2 * M_PI * u(g);
  double phi = acos(1 - 2 * u(g));
  return glm::vec3(
    sin(phi) * cos(theta),
    sin(phi) * sin(theta),
    cos(phi)
  );

}

int main( int argc, char* args[] ) {

  Tiny::view.ccw = false;
  Tiny::view.vsync = false;
  Tiny::window("Spherical Voronoi Diagram with Cubemap Texture", 800, 800);	//Open Window

  // Camera for Viewing Sphere

  cam::near = -100.0f;
  cam::far = 100.0f;
  cam::zoomrate *= 10.0f;
  cam::init(500.0f, cam::ORTHO);

  // Shader, Model for Rendering Sphere / Sampling from Cubemap

  Shader sphere({"shader/sphere.vs", "shader/sphere.fs"}, {"in_Position"});
  Icosphere icosahedron;

  // Cubemap and Cube Voronoi Shader

  Shader cubevoronoi({"shader/cubevoronoi.vs", "shader/cubevoronoi.gs", "shader/cubevoronoi.fs"}, {"in_Position"});
  Cubemap voronoi(512, 512);

  // View Matrices for Cubemap Geometry Shader

  glm::mat4 pointproj = glm::perspective(glm::radians(90.0f), 1.0f, 0.01f, 1.1f);
  std::vector<glm::mat4> views;
  views.push_back(pointproj*glm::lookAt(glm::vec3(0), glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
  views.push_back(pointproj*glm::lookAt(glm::vec3(0), glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
  views.push_back(pointproj*glm::lookAt(glm::vec3(0), glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
  views.push_back(pointproj*glm::lookAt(glm::vec3(0), glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
  views.push_back(pointproj*glm::lookAt(glm::vec3(0), glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
  views.push_back(pointproj*glm::lookAt(glm::vec3(0), glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

  // Istance Render Object

  // Random Centroids

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator (seed);

  std::vector<glm::vec3> centroids;
  std::vector<glm::vec3> offsets;
  for(size_t i = 0; i < (2 << 13); i++)
    centroids.push_back(spheresample(generator));
  offsets = centroids;

  // Buffer and Instance of Flat

  Buffer centroidbuf(offsets);

  Square3D flat;
  Instance instance(&flat);
  instance.bind<glm::vec3>("in_Centroid", &centroidbuf);

  // Loop

  const float R = 0.05f;

  Tiny::event.handler = cam::handler;
  Tiny::view.pipeline = [&](){

    voronoi.target();
    cubevoronoi.use();
    cubevoronoi.uniform("vp", views);
    cubevoronoi.uniform("R", R);
    instance.render();

    Tiny::view.target(glm::vec3(0));

    sphere.use();
    sphere.uniform("vp", cam::vp);	//View Projection Matrix
    sphere.texture("voronoicolor", voronoi.texture);
    sphere.texture("voronoidepth", voronoi.depth);
    icosahedron.render(GL_TRIANGLES);							  //Render Model with Lines

  };

  // Extra Stuff (Movement)

  float t = 0.0f;

  FastNoiseLite noise;
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalOctaves(8.0f);
  noise.SetFractalLacunarity(2.0f);
  noise.SetFractalGain(0.6f);
  noise.SetFrequency(1.0);

  Tiny::loop([&](){											//Extra Stuff (every tick)


    t += 0.01f;

    for(unsigned int i = 0; i < offsets.size(); i++){
      offsets[i].x = centroids[i].x + 0.5f*R*noise.GetNoise(centroids[i].x, centroids[i].y, t);
      offsets[i].y = centroids[i].y + 0.5f*R*noise.GetNoise(centroids[i].x, centroids[i].y, -t);
      offsets[i].z = centroids[i].z + 0.5f*R*noise.GetNoise(centroids[i].x, centroids[i].y, t);
    }
    centroidbuf.fill(offsets);


  });

  Tiny::quit();
}

#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <random>

#include "icosphere.h"

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
  Cubemap voronoi(256, 256);

  // View Matrices for Cubemap Geometry Shader

  float pointnear = 0.01f;
  float pointfar = 2.0f;

  glm::mat4 pointproj = glm::perspective(glm::radians(90.0f), 1.0f, pointnear, pointfar);
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
  for(size_t i = 0; i < 2048; i++)//(2 << 12); i++)
    centroids.push_back(spheresample(generator));

  // Buffer and Instance of Flat

  Buffer centroidbuf(centroids);

  Square3D flat;
  Instance instance(&flat);
  instance.bind<glm::vec3>("in_Centroid", &centroidbuf);

  // Loop

  Tiny::event.handler = cam::handler;
  Tiny::view.pipeline = [&](){

    voronoi.target();
    cubevoronoi.use();
    cubevoronoi.uniform("vp", views);
    cubevoronoi.uniform("far", pointfar);
    cubevoronoi.uniform("R", 0.2);
    instance.render();

    Tiny::view.target(glm::vec3(0));

    sphere.use();
    sphere.uniform("vp", cam::vp);	//View Projection Matrix
    sphere.texture("voronoicolor", voronoi.texture);
    sphere.texture("voronoidepth", voronoi.depth);
    icosahedron.render(GL_TRIANGLES);							  //Render Model with Lines

  };

  Tiny::loop([&](){											//Extra Stuff (every tick)

  });

  Tiny::quit();
}

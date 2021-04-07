#include "noise/noise.h"

int SEED = 10;
double scale = 30.0;
double heightmap[GRIDSIZE][GRIDSIZE] = {0.0};
glm::vec2 dim = glm::vec2(GRIDSIZE);
noise::module::Perlin perlin;
std::function<void(Model* m)> _construct;

void setup(){

  srand(time(NULL));
  SEED = rand();

  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.6);

  float min, max = 0.0;
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      heightmap[i][j] = perlin.GetValue(i*(1.0/dim.x), j*(1.0/dim.y), SEED);
      if(heightmap[i][j] > max) max = heightmap[i][j];
      if(heightmap[i][j] < min) min = heightmap[i][j];
    }
  }

  //Normalize
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      heightmap[i][j] = (heightmap[i][j] - min)/(max - min);
    }
  }

  _construct = [&](Model* h){
    //Loop over all positions and add the triangles!
    for(int i = 0; i < dim.x-1; i++){
      for(int j = 0; j < dim.y-1; j++){

        //Add to Position Vector
        glm::vec3 a = glm::vec3(i, scale*heightmap[i][j], j);
        glm::vec3 b = glm::vec3(i+1, scale*heightmap[i+1][j], j);
        glm::vec3 c = glm::vec3(i, scale*heightmap[i][j+1], j+1);
        glm::vec3 d = glm::vec3(i+1, scale*heightmap[i+1][j+1], j+1);

        //UPPER TRIANGLE

        //Add Indices
        h->indices.push_back(h->positions.size()/3+0);
        h->indices.push_back(h->positions.size()/3+1);
        h->indices.push_back(h->positions.size()/3+2);
        h->indices.push_back(h->positions.size()/3+0);

        h->add(h->positions, a);
        h->add(h->positions, b);
        h->add(h->positions, c);

        glm::vec3 n1 = glm::cross(a-b, c-b);

        for(int i = 0; i < 3; i++)
          h->add(h->normals, n1);

        //Lower Triangle
        h->indices.push_back(h->positions.size()/3+0);
        h->indices.push_back(h->positions.size()/3+1);
        h->indices.push_back(h->positions.size()/3+2);
        h->indices.push_back(h->positions.size()/3+0);

        h->add(h->positions, d);
        h->add(h->positions, c);
        h->add(h->positions, b);

        glm::vec3 n2 = glm::cross(d-c, b-c);

        for(int i = 0; i < 3; i++)
          h->add(h->normals, n2);

      }
    }
  };


};

#include "FastNoiseLite.h"

int SEED = 10;
double scale = 30.0;
double heightmap[GRIDSIZE][GRIDSIZE] = {0.0};
glm::vec2 dim = glm::vec2(GRIDSIZE);

void setup(){

  srand(time(NULL));
  SEED = rand();

  FastNoiseLite noise;
  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalOctaves(8.0f);
  noise.SetFractalLacunarity(2.0f);
  noise.SetFractalGain(0.6f);
  noise.SetFrequency(1.0);

  float min, max = 0.0;
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      heightmap[i][j] = noise.GetNoise((float)(i)*(1.0f/dim.x), (float)(j)*(1.0f/dim.y), (float)(SEED%1000));
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

};

void construct(Buffer& positions, Buffer& normals, Buffer& indices){
  //Fill the Buffers!

  std::vector<int> indbuf;
  std::vector<float> posbuf, norbuf;

  std::function<void(std::vector<float>&, glm::vec3)> add = [](std::vector<float>& v, glm::vec3 p){
    v.push_back(p.x);
    v.push_back(p.y);
    v.push_back(p.z);
  };

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
      indbuf.push_back(posbuf.size()/3+0);
      indbuf.push_back(posbuf.size()/3+1);
      indbuf.push_back(posbuf.size()/3+2);
      indbuf.push_back(posbuf.size()/3+0);

      add(posbuf, a);
      add(posbuf, b);
      add(posbuf, c);

      glm::vec3 n1 = glm::cross(a-b, c-b);

      for(int i = 0; i < 3; i++)
        add(norbuf, n1);

        indbuf.push_back(posbuf.size()/3+0);
        indbuf.push_back(posbuf.size()/3+1);
        indbuf.push_back(posbuf.size()/3+2);
        indbuf.push_back(posbuf.size()/3+0);

        add(posbuf, d);
        add(posbuf, c);
        add(posbuf, b);

      glm::vec3 n2 = glm::cross(d-c, b-c);

      for(int i = 0; i < 3; i++)
        add(norbuf, n2);

    }
  }

  indices.fill<int>(indbuf);
  positions.fill<float>(posbuf);
  normals.fill<float>(norbuf);

}

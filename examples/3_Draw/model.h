#include "noise/noise.h"

//Model Stuff
int SEED = 10;
double scale = 30.0;
double heightmap[64*64] = {0.0};
glm::vec2 dim = glm::vec2(64);
noise::module::Perlin perlin;

//View Stuff
const int WIDTH = 1200;
const int HEIGHT = 800;

void setup(){
  //Random Seed
  srand(time(NULL));
  SEED = rand();

  perlin.SetOctaveCount(8);
  perlin.SetFrequency(1.0);
  perlin.SetPersistence(0.6);

  float min, max = 0.0;
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      heightmap[i*(int)dim.y+j] = perlin.GetValue(i*(1.0/dim.x), j*(1.0/dim.y), SEED);
      if(heightmap[i*(int)dim.y+j] > max) max = heightmap[i*(int)dim.y+j];
      if(heightmap[i*(int)dim.y+j] < min) min = heightmap[i*(int)dim.y+j];
    }
  }

  //Normalize
  for(int i = 0; i < dim.x; i++){
    for(int j = 0; j < dim.y; j++){
      //Normalize to (0, 1) scale.
      heightmap[i*(int)dim.y+j] = (heightmap[i*(int)dim.y+j] - min)/(max - min);
    }
  }
};

// Event Handler
std::function<void()> eventHandler = [&](){

};

#include <random>
#include <vector>

namespace sample{

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(0.0, 1.0);

  float uniform(){
    return dis(gen);
  }

  float uniform(float a, float b){
    return dis(gen)*(a-b)+b;
  }

  //Single point at position towards p with radius R
  bool disc(std::vector<glm::vec2>& set, float R, glm::vec2 p, glm::vec2 a, glm::vec2 b){

    if(set.empty()){
      set.push_back(p);
      return true;
    }

    const float length = R/sqrt(2);
    const int NX = ceil((b.x-a.x)/length);
    const int NY = ceil((b.y-a.y)/length);
    int* grid = new int[NX*NY];
    memset(grid, 0, NX*NY*sizeof(int));

    std::function<void(glm::vec2, int)> addGrid = [&](glm::vec2 s, int val){
      glm::ivec2 ind = glm::vec2(NX, NY)*(s-a)/(b-a); //Indices
      grid[ind.x*NY+ind.y] = val; //Set Indices in Neighborhood
    };

    //Set is Not Empty - Fill Grid
    for(unsigned int i = 0; i < set.size(); i++)
      addGrid(set[i], i+1);

    //Find the Nearest Guy - Attempt to Place Nearby
    int near = 0; float dist = glm::distance(a, b);
    for(unsigned int i = 0; i < set.size(); i++){
      if(glm::distance(set[i], p) < dist){
        dist = glm::distance(set[i], p);
        near = i;
      }
    }

    //Generate Nearby Point
    int tries = 16;
    while(tries > 0){
      tries--;

      float nr = uniform(R, 2*R);
      float nt = uniform(0, 2*3.14159265);

      glm::vec2 npos = set[near]+nr*glm::vec2(cos(nt), sin(nt));
      if(glm::any(glm::lessThan(npos, glm::vec2(a))) || glm::any(glm::greaterThanEqual(npos, glm::vec2(b))))
        continue;

      glm::ivec2 ind = glm::vec2(NX, NY)*(npos-a)/(b-a); //Indices

      bool free = true;
      for(int i = ind.x-2; i <= ind.x+2 && free; i++){
        for(int j = ind.y-2; j <= ind.y+2 && free; j++){

          if(i < 0 || i >= NX || j < 0 || j >= NY)
            continue;

          //Non-Zero Entry,and Too Close
          if(grid[i*NY+j] > 0){
            if(glm::distance(set[grid[i*NY+j]-1], npos) < R){
              free = false;
              continue;
            }
          }
        }
      }

      if(free){
        set.push_back(npos);
        addGrid(set.back(), set.size());
        return true;
      }
    }
    return false;
  }

  //Sample Set of Points in Region
  void disc(std::vector<glm::vec2>& set, int K, glm::vec2 a, glm::vec2 b){

    float r = sqrt(glm::dot(b-a, b-a)/3.14159265f/K);
    const float length = r/sqrt(2);     //Edge Length
    const int NX = ceil((b.x-a.x)/length);
    const int NY = ceil((b.y-a.y)/length);
    int* grid = new int[NX*NY];
    memset(grid, 0, NX*NY*sizeof(int));

    //Grid Addition Function
    std::function<void(glm::vec2, int)> addGrid = [&](glm::vec2 s, int val){
      glm::ivec2 ind = glm::vec2(NX, NY)*(s-a)/(b-a); //Indices
      grid[ind.x*NY+ind.y] = val; //Set Indices in Neighborhood
    };

    //Setup Grid
    if(set.empty()){
      set.push_back(0.5f*(a+b)); //Center
      addGrid(set.back(), set.size());
      K--;
    }
    else for(unsigned int i = 0; i < set.size(); i++)
      addGrid(set[i], i+1);

    //Now Generate New Samples by Iterating over the Set
    int tries = 0;

    while(K > 0){

      if(tries > set.size())
        break;

      //Sample Uniformly from Set
      int n = uniform(0, set.size());

      //Generate Sample Surrounding It
      float nr = uniform(r, 2*r);
      float nt = uniform(0, 2*3.14159265);

      //New Sample Position, Grid Index
      glm::vec2 npos = set[n]+nr*glm::vec2(cos(nt), sin(nt));
      if(glm::any(glm::lessThan(npos, glm::vec2(a))) || glm::any(glm::greaterThanEqual(npos, glm::vec2(b))))
        continue;

      glm::ivec2 ind = glm::vec2(NX, NY)*(npos-a)/(b-a); //Indices

      bool free = true;
      for(int i = ind.x-2; i <= ind.x+2 && free; i++){
        for(int j = ind.y-2; j <= ind.y+2 && free; j++){

          if(i < 0 || i >= NX || j < 0 || j >= NY)
            continue;

          //Non-Zero Entry,and Too Close
          if(grid[i*NY+j] > 0){
            if(glm::distance(set[grid[i*NY+j]-1], npos) < r){
              free = false;
              continue;
            }
          }
        }
      }

      if(!free){
        tries++;
        continue;
      }

      //Place Point
      set.push_back(npos);
      addGrid(set.back(), set.size());
      tries = 0;
      K--;
    }
    delete[] grid;
  }

  //Those functions could probably be improved. Not sure how right now. Recursiveness?
}

/*
================================================================================
                                    Chunk
================================================================================

Chunks are data structures containing a random array of bytes.

The goal is to randomly change, mesh and render them extremely quickly.

Ideally we also use a memory pool for the chunks themselves.

*/

#include <functional>
#include "FastNoiseLite.h"

#define CHUNKSIZE 16
#define CHUNKVOL (CHUNKSIZE*CHUNKSIZE*CHUNKSIZE)

using namespace glm;

vec3 getPos(int index, vec3 s = vec3(CHUNKSIZE)){
  int z = index % (int)s.x;
  int y = (int)(index / s.x) % (int)s.y;
  int x = index / ( s.x * s.y );
  return glm::vec3(x, y, z);
}

int getInd(vec3 p, vec3 s = vec3(CHUNKSIZE)){
  if(glm::all(lessThan(p, s)) && all(glm::greaterThanEqual(p, vec3(0)))){
    //return p.x*s.y*s.z+p.y*s.z+p.z;
    return p.z*s.y*s.z+p.y*s.z+p.x;
  }
  return -1;
}

enum BlockType: unsigned char {
  BLOCK_NONE,
  BLOCK_RED,
  BLOCK_GREEN,
  BLOCK_BLUE,
  BLOCK_ANY
};

namespace block{
  vec3 getColor(BlockType _type){
    //Switch the value and return a vector
    switch(_type){
      case BLOCK_NONE:
        return vec3(1,1,1);
      case BLOCK_RED:
        return vec3(1,1,0);
      case BLOCK_GREEN:
        return vec3(1,0,1);
      case BLOCK_BLUE:
        return vec3(0,1,1);
      default:
        return vec3(1,1,1);
    }
  }
}

class Chunk {
public:

Chunk(){
  data = new BlockType[CHUNKVOL];
  for(int i = 0; i < CHUNKVOL; i++)
    data[i] = BLOCK_NONE;

}

Chunk(ivec3 p):Chunk(){
  pos = p;

  noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  noise.SetFractalType(FastNoiseLite::FractalType_FBm);
  noise.SetFractalOctaves(8.0f);
  noise.SetFractalLacunarity(2.0f);
  noise.SetFractalGain(0.6f);
  noise.SetFrequency(1.0);

  update();
}

FastNoiseLite noise;

float t = 0.0f;

void update(){

  for(size_t i = 0; i < CHUNKVOL; i++){
    vec3 p = (getPos(i, vec3(CHUNKSIZE))+(vec3)(CHUNKSIZE*pos))/vec3(CHUNKSIZE*3);
    if(noise.GetNoise(p.x, p.y+t, p.z) > 0.0)
      data[i] = BLOCK_RED;
    else data[i] = BLOCK_NONE;

  }
  t+= 0.1f;
//  size_t i = rand()%CHUNKVOL;
}
/*
void update(){

  for(size_t i = 0; i < CHUNKVOL; i++){
    if(rand()%10 == 0)
      data[i] = BlockType(1+rand()%3);
    else data[i] = BLOCK_NONE;
  }
//  size_t i = rand()%CHUNKVOL;

}
*/

BlockType* data;
ivec3 pos = ivec3(0);
int quadsize, quadstart;

static int LOD;
static int QUAD;

array<uint*, 6> faces;

int getIndex(vec3 _p){
  //Return the Correct Index
  if(all(lessThan(_p, vec3(CHUNKSIZE)))){
    return _p.x*CHUNKSIZE*CHUNKSIZE+_p.y*CHUNKSIZE+_p.z;
  }
  return 0;
}

BlockType getPosition(vec3 _p){
  return (BlockType)data[getIndex(_p)];
}

};

int Chunk::LOD = 1;
int Chunk::QUAD = 3500;

/*
================================================================================
                          Chunk Meshing Algorithms
================================================================================
*/

namespace chunkmesh {
using namespace std;

function<void(Chunk*, Buffer*, Buffer*, Buffer*, Buffer*)> greedy = []( Chunk* c, Buffer* ind, Buffer* pos, Buffer* nor, Buffer* col){

  int LOD = Chunk::LOD;
  int CHLOD = CHUNKSIZE/LOD;
  vec3 p = c->pos*ivec3(CHUNKSIZE/LOD);

  int u, v, w;
  int n;

  BlockType* mask = new BlockType[CHUNKSIZE*CHUNKSIZE/LOD/LOD];
  BlockType current, facing;
  int s;

  int quads = 0;

  vec3 color;

  vector<GLuint> indices;
  vector<GLfloat> positions, colors, normals;

  for(int d = 0; d < 6; d++){

    u = (d/2+0)%3;      //u = 0, 0, 1, 1, 2, 2      //Dimension indices
    v = (d/2+1)%3;      //v = 1, 1, 2, 2, 0, 0
    w = (d/2+2)%3;      //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    n = 2*(d%2)-1;      //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    c->quadsize = 0;

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth

      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_NONE;                                                  //Set to Air

          current = c->getPosition((float)LOD*vec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_NONE) continue;

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->getPosition((float)LOD*vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_NONE){
            mask[s] = current;
          }

        }
      }

      int width = 1, height = 1;
      bool quaddone;

      for(x[v] = 0; x[v] < CHLOD; x[v]++){            //Evaluate Mask
        for(x[w] = 0; x[w] < CHLOD; x[w]+=width){   //Permissible Skip

          width = height = 1;       //Current Quad Dimensions

          s = x[w] + x[v]*CHLOD;    //Current Slice Index
          current = mask[s];        //Current Block Type

          if(current == BLOCK_NONE)  //We don't mesh air
            continue;

          while(mask[s+width] == current && x[w] + width < CHLOD)
            width++;

          quaddone = false;
          for(height = 1; x[v] + height < CHLOD; height++){   //Find Height

            for(int k = 0; k < width; k++){                   //Iterate Over Width
              if(mask[s+k+height*CHLOD] != current) {
                quaddone = true;
                break;
              }
            }
            if(quaddone) break;
          }

          for(int l = x[v]; l < x[v] + height; l++)   //Zero the Mask
          for(int k = x[w]; k < x[w] + width; k++)
            mask[k+l*CHLOD] = BLOCK_NONE;

          int du[3] = {0}; du[v] = height;
          int dv[3] = {0}; dv[w] = width;

          if(n < 0){

            indices.push_back(positions.size()/3+0);
            indices.push_back(positions.size()/3+2);
            indices.push_back(positions.size()/3+1);
            indices.push_back(positions.size()/3+3);
            indices.push_back(positions.size()/3+2);
            indices.push_back(positions.size()/3+0);

            //Some need to go clock-wise, others need to go counterclockwise.
            positions.push_back((p.x+x[0]-0.5)*(float)LOD);
            positions.push_back((p.y+x[1]-0.5)*(float)LOD);
            positions.push_back((p.z+x[2]-0.5)*(float)LOD);
            //Vertex 2
            positions.push_back((p.x+x[0]+du[0]-0.5)*(float)LOD);
            positions.push_back((p.y+x[1]+du[1]-0.5)*(float)LOD);
            positions.push_back((p.z+x[2]+du[2]-0.5)*(float)LOD);
            //Vertex 3
            positions.push_back((p.x+x[0]+du[0]+dv[0]-0.5)*(float)LOD);
            positions.push_back((p.y+x[1]+du[1]+dv[1]-0.5)*(float)LOD);
            positions.push_back((p.z+x[2]+du[2]+dv[2]-0.5)*(float)LOD);
            //Vertex 4
            positions.push_back((p.x+x[0]+dv[0]-0.5)*(float)LOD);
            positions.push_back((p.y+x[1]+dv[1]-0.5)*(float)LOD);
            positions.push_back((p.z+x[2]+dv[2]-0.5)*(float)LOD);

          }
          else{

            indices.push_back(positions.size()/3+0);
            indices.push_back(positions.size()/3+2);
            indices.push_back(positions.size()/3+1);
            indices.push_back(positions.size()/3+1);
            indices.push_back(positions.size()/3+3);
            indices.push_back(positions.size()/3+0);
            //Vertex 0
            positions.push_back((p.x+x[0]-0.5+y[0])*(float)LOD);
            positions.push_back((p.y+x[1]-0.5+y[1])*(float)LOD);
            positions.push_back((p.z+x[2]-0.5+y[2])*(float)LOD);
            //Vertex 1
            positions.push_back((p.x+x[0]+du[0]+dv[0]-0.5+y[0])*(float)LOD);
            positions.push_back((p.y+x[1]+du[1]+dv[1]-0.5+y[1])*(float)LOD);
            positions.push_back((p.z+x[2]+du[2]+dv[2]-0.5+y[2])*(float)LOD);
            //Vertex 2
            positions.push_back((p.x+x[0]+du[0]-0.5+y[0])*(float)LOD);
            positions.push_back((p.y+x[1]+du[1]-0.5+y[1])*(float)LOD);
            positions.push_back((p.z+x[2]+du[2]-0.5+y[2])*(float)LOD);
            //Vertex 3
            positions.push_back((p.x+x[0]+dv[0]-0.5+y[0])*(float)LOD);
            positions.push_back((p.y+x[1]+dv[1]-0.5+y[1])*(float)LOD);
            positions.push_back((p.z+x[2]+dv[2]-0.5+y[2])*(float)LOD);

          }

          color = block::getColor(current);

          for(int l = 0; l < 4; l++){
            colors.push_back(color.x);
            colors.push_back(color.y);
            colors.push_back(color.z);
            colors.push_back(1.0);
            normals.push_back(q[0]);
            normals.push_back(q[1]);
            normals.push_back(q[2]);
          }

          c->quadsize++;

          //Next Quad
        }
      }
      //Next Slice
    }
    //Next Surface Orientation
    quads += c->quadsize;
  }
  delete[] mask;
//  std::cout<<"QUADS: "<<quads<<std::endl;

  ind->fill(indices);
  pos->fill(positions);
  nor->fill(normals);
  col->fill(colors);

};





function<void(Chunk*, Vertexpool<Vertex>*)> greedypool = [](Chunk* c, Vertexpool<Vertex>* vertpool){

  int LOD = Chunk::LOD;
  int CHLOD = CHUNKSIZE/LOD;
  vec3 p = c->pos*ivec3(CHUNKSIZE/LOD);

  int u, v, w;
  int n;

  BlockType* mask = new BlockType[CHUNKSIZE*CHUNKSIZE/LOD/LOD];
  BlockType current, facing;
  int s;

  uint* section;

  int quads = 0;

  for(int d = 0; d < 6; d++){

  //  std::cout<<"Chunk D-Loop ";
  //  timer::benchmark<std::chrono::microseconds>([&](){

    u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2      //Dimension indices
    v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    int n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    c->quadsize = 0;

    section = vertpool->section(Chunk::QUAD, d, (vec3)c->pos + vec3(0.5, 0.5, 0.5)*vec3(q[0], q[1], q[2]));

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth

      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_NONE;                                                  //Set to Air

          current = c->getPosition((float)LOD*vec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_NONE) continue;

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->getPosition((float)LOD*vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_NONE)
            mask[s] = current;
          // else mask[s] = BLOCK_ANY;

        }
      }

      int width = 1, height = 1;
      bool quaddone;
      vec3 color;

      for(x[v] = 0; x[v] < CHLOD; x[v]++){            //Evaluate Mask
        for(x[w] = 0; x[w] < CHLOD; x[w] += width){   //Permissible Skip

          width = height = 1;       //Current Quad Dimensions

          s = x[w] + x[v]*CHLOD;    //Current Slice Index
          current = mask[s];        //Current Block Type

          if(current == BLOCK_NONE /* || current == BLOCK_ANY */ )  //We don't mesh air
            continue;

          while(x[w] + width < CHLOD && (mask[s+width] == current /* || mask[s+width] == BLOCK_ANY */ ))
            width++;

          quaddone = false;
          for(height = 1; x[v] + height < CHLOD; height++){   //Find Height

            for(int k = 0; k < width; k++){                   //Iterate Over Width
              if(mask[s+k+height*CHLOD] != current /* && mask[s+k+height*CHLOD] != BLOCK_ANY */ ) {
                quaddone = true;
                break;
              }
            }
            if(quaddone) break;
          }

          for(int l = x[v]; l < x[v] + height; l++)   //Zero the Mask
          for(int k = x[w]; k < x[w] + width; k++)
            mask[k+l*CHLOD] = BLOCK_NONE;

          vec3 px = p+vec3(x[0], x[1], x[2]);
          vec3 qq = vec3(q[0], q[1], q[2]);

          int du[3] = {0}; du[v] = height;
          int dv[3] = {0}; dv[w] = width;

          color = block::getColor(current);


          if(n < 0){

            vertpool->fill(section, c->quadsize*4+0,
              vec3( (p.x+x[0]-0.5)*(float)LOD,
                    (p.y+x[1]-0.5)*(float)LOD,
                    (p.z+x[2]-0.5)*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+1,
              vec3( (p.x+x[0]+du[0]+dv[0]-0.5)*(float)LOD,
                    (p.y+x[1]+du[1]+dv[1]-0.5)*(float)LOD,
                    (p.z+x[2]+du[2]+dv[2]-0.5)*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+2,
              vec3( (p.x+x[0]+du[0]-0.5)*(float)LOD,
                    (p.y+x[1]+du[1]-0.5)*(float)LOD,
                    (p.z+x[2]+du[2]-0.5)*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+3,
              vec3( (p.x+x[0]+dv[0]-0.5)*(float)LOD,
                    (p.y+x[1]+dv[1]-0.5)*(float)LOD,
                    (p.z+x[2]+dv[2]-0.5)*(float)LOD),
              qq, color);

          }
          else{

            vertpool->fill(section, c->quadsize*4+0,
              vec3( (p.x+x[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]-0.5+y[2])*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+1,
              vec3( (p.x+x[0]+du[0]+dv[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]+du[1]+dv[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]+du[2]+dv[2]-0.5+y[2])*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+2,
              vec3( (p.x+x[0]+du[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]+du[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]+du[2]-0.5+y[2])*(float)LOD),
              qq, color);

            vertpool->fill(section, c->quadsize*4+3,
              vec3( (p.x+x[0]+dv[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]+dv[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]+dv[2]-0.5+y[2])*(float)LOD),
              qq, color);

          }

          c->quadsize++;
          //Next Quad
        }
      }
      //Next Slice
    }

    vertpool->resize(section, c->quadsize*6);
    c->faces[d] = section;
    quads += c->quadsize;
  //  });

  //Next Surface Orientation
  }
  //std::cout<<"QUADS: "<<quads<<std::endl;

  delete[] mask;

};

}

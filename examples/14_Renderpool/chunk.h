/*
================================================================================
                                    Chunk
================================================================================

Chunks are data structures containing a random array of bytes.

The goal is to randomly change, mesh and render them extremely quickly.

Ideally we also use a memory pool for the chunks themselves.

*/

#include <functional>

#define CHUNKSIZE 16
#define CHUNKVOL CHUNKSIZE*CHUNKSIZE*CHUNKSIZE

using namespace glm;

enum BlockType: unsigned char {
  BLOCK_NONE,
  BLOCK_RED,
  BLOCK_GREEN,
  BLOCK_BLUE
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
  randomize();
}

void randomize(){
  for(size_t i = 0; i < CHUNKVOL; i++){
    if(rand()%5 < 4) data[i] = BLOCK_NONE;
    else data[i] = BlockType(rand()%4);
  }
}

~Chunk(){
//  delete data;
}

BlockType* data;
ivec3 pos = ivec3(0);
int quadsize, quadstart;

static int LOD;
static int QUAD;

int faces[6];

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

function<void(Model* , Chunk*)> greedy = [](Model* m, Chunk* c){

  m->pos = c->pos;

  c->quadsize = 0;

  int LOD = Chunk::LOD;
  int CHLOD = CHUNKSIZE/LOD;
  vec3 p = c->pos*ivec3(CHUNKSIZE/LOD);

  for(int d = 0; d < 6; d++){

    int u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2      //Dimension m->indices
    int v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    int w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    int n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    BlockType* mask = new BlockType[CHUNKSIZE*CHUNKSIZE/LOD/LOD];
    BlockType current, facing;
    int s;

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth

    //  bool foundmask = false;

      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_NONE;                                                  //Set to Air

          current = c->getPosition((float)LOD*vec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_NONE) continue;

          //Skip Non-Cubic Elements in the Chunk! (every iteration)
        //  if(!block::isCubic(atPos)){
            //If it is our first pass, also add it to the meshing editBuffer.
            //IGNORE THIS FOR LARGE LOD
        //    if(d == 0) temp.add(vec3(x[0], x[1], x[2]), atPos, false);
      //      continue;
        //  }

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
  //          foundmask = true;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->getPosition((float)LOD*vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_NONE){
            mask[s] = current;
  //          foundmask = true;
          }

        }
      }

  //    if(!foundmask) continue;

      int width = 1, height = 1;
      bool quaddone;
      vec3 color;

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

          //Add Quad to Model
//          int N = m->positions.size()/3;

          if(n < 0){

            m->indices.push_back(m->positions.size()/3+0);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+3);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+0);

            //Some need to go clock-wise, others need to go counterclockwise.
            m->positions.push_back((p.x+x[0]-0.5)*(float)LOD);
            m->positions.push_back((p.y+x[1]-0.5)*(float)LOD);
            m->positions.push_back((p.z+x[2]-0.5)*(float)LOD);
            //Vertex 2
            m->positions.push_back((p.x+x[0]+du[0]-0.5)*(float)LOD);
            m->positions.push_back((p.y+x[1]+du[1]-0.5)*(float)LOD);
            m->positions.push_back((p.z+x[2]+du[2]-0.5)*(float)LOD);
            //Vertex 3
            m->positions.push_back((p.x+x[0]+du[0]+dv[0]-0.5)*(float)LOD);
            m->positions.push_back((p.y+x[1]+du[1]+dv[1]-0.5)*(float)LOD);
            m->positions.push_back((p.z+x[2]+du[2]+dv[2]-0.5)*(float)LOD);
            //Vertex 4
            m->positions.push_back((p.x+x[0]+dv[0]-0.5)*(float)LOD);
            m->positions.push_back((p.y+x[1]+dv[1]-0.5)*(float)LOD);
            m->positions.push_back((p.z+x[2]+dv[2]-0.5)*(float)LOD);

          }
          else{

            m->indices.push_back(m->positions.size()/3+0);
            m->indices.push_back(m->positions.size()/3+2);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+1);
            m->indices.push_back(m->positions.size()/3+3);
            m->indices.push_back(m->positions.size()/3+0);
            //Vertex 0
            m->positions.push_back((p.x+x[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((p.y+x[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((p.z+x[2]-0.5+y[2])*(float)LOD);
            //Vertex 1
            m->positions.push_back((p.x+x[0]+du[0]+dv[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((p.y+x[1]+du[1]+dv[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((p.z+x[2]+du[2]+dv[2]-0.5+y[2])*(float)LOD);
            //Vertex 2
            m->positions.push_back((p.x+x[0]+du[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((p.y+x[1]+du[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((p.z+x[2]+du[2]-0.5+y[2])*(float)LOD);
            //Vertex 3
            m->positions.push_back((p.x+x[0]+dv[0]-0.5+y[0])*(float)LOD);
            m->positions.push_back((p.y+x[1]+dv[1]-0.5+y[1])*(float)LOD);
            m->positions.push_back((p.z+x[2]+dv[2]-0.5+y[2])*(float)LOD);

          }

          color = block::getColor(current);

          for(int l = 0; l < 4; l++){
            m->add(m->colors, vec4(color, 1.0));
            m->add(m->normals, vec3(q[0], q[1], q[2]));
          }

          c->quadsize++;

          //Next Quad
        }
      }
      //Next Slice
    }
    //Next Surface Orientation
    delete[] mask;
  }

};












function<void(Chunk*, Renderpool<Vertex>*)> greedypool = [](Chunk* c, Renderpool<Vertex>* vertpool){

  int LOD = Chunk::LOD;
  int CHLOD = CHUNKSIZE/LOD;
  vec3 p = c->pos*ivec3(CHUNKSIZE/LOD);

  int quadsize;
  int section;

  for(int d = 0; d < 6; d++){

    int u = (d/2+0)%3;  //u = 0, 0, 1, 1, 2, 2      //Dimension m->indices
    int v = (d/2+1)%3;  //v = 1, 1, 2, 2, 0, 0
    int w = (d/2+2)%3;  //w = 2, 2, 0, 0, 1, 1

    int x[3] = {0};
    int q[3] = {0};
    int y[3] = {0};

    int n = 2*(d%2)-1;  //Normal Direction
    q[u] = n;           //Normal Vector
    y[u] = 1;           //Simple Vector

    quadsize = 0;
    section = vertpool->section(Chunk::QUAD, d, c->pos);

    BlockType* mask = new BlockType[CHUNKSIZE*CHUNKSIZE/LOD/LOD];
    BlockType current, facing;
    int s;

    for(x[u] = 0; x[u] < CHLOD; x[u]++){       //Loop Over Depth

    //  bool foundmask = false;

      for(x[v] = 0; x[v] < CHLOD; x[v]++){     //Loop Over Slice
        for(x[w] = 0; x[w] < CHLOD; x[w]++){

          s = x[w] + x[v]*CHLOD;                                    //Slice Index
          mask[s] = BLOCK_NONE;                                                  //Set to Air

          current = c->getPosition((float)LOD*vec3(x[0],x[1],x[2]));   //Position at one corner!

          if(current == BLOCK_NONE) continue;

          //Skip Non-Cubic Elements in the Chunk! (every iteration)
        //  if(!block::isCubic(atPos)){
            //If it is our first pass, also add it to the meshing editBuffer.
            //IGNORE THIS FOR LARGE LOD
        //    if(d == 0) temp.add(vec3(x[0], x[1], x[2]), atPos, false);
      //      continue;
        //  }

          //Basically, we are facing out of the chunk, so we do take over the surface.
          if(x[u] + q[u] < 0 || x[u] + q[u] >= CHLOD){
            mask[s] = current;
  //          foundmask = true;
            continue;
          }

          //Now see if we should remove this mask element or not, i.e. not visible!
          facing = c->getPosition((float)LOD*vec3(x[0]+q[0],x[1]+q[1],x[2]+q[2]));

          //Make sure that the facing block can be air or non-cubic!
          if(facing == BLOCK_NONE){
            mask[s] = current;
  //          foundmask = true;
          }

        }
      }

  //    if(!foundmask) continue;

      int width = 1, height = 1;
      bool quaddone;
      vec3 color;

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

          color = block::getColor(current);

          if(n < 0){

            vertpool->fill(section, quadsize*4+0,
              vec3( (p.x+x[0]-0.5)*(float)LOD,
                    (p.y+x[1]-0.5)*(float)LOD,
                    (p.z+x[2]-0.5)*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

            vertpool->fill(section, quadsize*4+1,
              vec3( (p.x+x[0]+du[0]+dv[0]-0.5)*(float)LOD,
                    (p.y+x[1]+du[1]+dv[1]-0.5)*(float)LOD,
                    (p.z+x[2]+du[2]+dv[2]-0.5)*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

            vertpool->fill(section, quadsize*4+2,
              vec3( (p.x+x[0]+du[0]-0.5)*(float)LOD,
                    (p.y+x[1]+du[1]-0.5)*(float)LOD,
                    (p.z+x[2]+du[2]-0.5)*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

            vertpool->fill(section, quadsize*4+3,
              vec3( (p.x+x[0]+dv[0]-0.5)*(float)LOD,
                    (p.y+x[1]+dv[1]-0.5)*(float)LOD,
                    (p.z+x[2]+dv[2]-0.5)*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

          }
          else{

            vertpool->fill(section, quadsize*4+0,
              vec3( (p.x+x[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]-0.5+y[2])*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

            vertpool->fill(section, quadsize*4+1,
              vec3( (p.x+x[0]+du[0]+dv[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]+du[1]+dv[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]+du[2]+dv[2]-0.5+y[2])*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

            vertpool->fill(section, quadsize*4+2,
              vec3( (p.x+x[0]+du[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]+du[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]+du[2]-0.5+y[2])*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

            vertpool->fill(section, quadsize*4+3,
              vec3( (p.x+x[0]+dv[0]-0.5+y[0])*(float)LOD,
                    (p.y+x[1]+dv[1]-0.5+y[1])*(float)LOD,
                    (p.z+x[2]+dv[2]-0.5+y[2])*(float)LOD),
              vec3(q[0], q[1], q[2]),
              color);

          }

          quadsize++;
          //Next Quad
        }
      }
      //Next Slice
    }

    vertpool->shrink(section, quadsize*6);
    c->faces[d] = section;
    delete[] mask;

    //Next Surface Orientation
  }

};











}

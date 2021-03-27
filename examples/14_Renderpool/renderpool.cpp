/*
================================================================================
                      TinyEngine Vertex Pooling
================================================================================

Idea: Use multi indirect rendering, combined with a memory pool, to render multiple
different objects with the same vertex format extremely quickly and dynamically.

For multiple objects, we store vertices in an interleaved format in a single VBO.
All vertices for these objects have the same format.

The VBO is persistently mapped so that we can utilize the pointer in a memory pool
to distribute vertices to object constructors as they are needed.

For every object, we create one (or more) indirect drawing commands specifying
the location in a single VBO where the object is located. Ideally, the memory is
consecutive but can theoretically be split based on the primitive type.

Removing objects requires removing the indirect drawing entry and returning
the vertices to the memory pool managing the VBO.

Potential Improvements:
- Full Instancing Support
- Full Indexing Support

*/

#include <list>

//Vertex Format

/*
struct Vertex{

	Vertex(glm::vec3 p, glm::vec3 n, glm::vec4 c){
		position[0] = p.x;
		position[1] = p.y;
		position[2] = p.z;
		normal[0] = n.x;
		normal[1] = n.y;
		normal[2] = n.z;
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
		color[3] = c.w;
	}

	float position[3];
	float normal[3];
	float color[4];

  static void format(int vbo){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(2, 4, GL_FLOAT, GL_FALSE, 0);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 1);
    glVertexAttribBinding(2, 2);

    glBindVertexBuffer(0, vbo, offsetof(Vertex, position), sizeof(Vertex));		//Internal Offset vs. Full Offset
    glBindVertexBuffer(1, vbo, offsetof(Vertex, normal), sizeof(Vertex));
    glBindVertexBuffer(2, vbo, offsetof(Vertex, color), sizeof(Vertex));
  }

};
*/

/*
struct Vertex{

	Vertex(glm::vec3 p, glm::vec3 n, glm::vec3 c){
		position[0] = p.x;
		position[1] = p.y;
		position[2] = p.z;
		normal[0] = n.x;
		normal[1] = n.y;
		normal[2] = n.z;
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
	}

	float position[3];
	float normal[3];
	float color[3];

  static void format(int vbo){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(2, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 1);
    glVertexAttribBinding(2, 2);

    glBindVertexBuffer(0, vbo, offsetof(Vertex, position), sizeof(Vertex));		//Internal Offset vs. Full Offset
    glBindVertexBuffer(1, vbo, offsetof(Vertex, normal), sizeof(Vertex));
    glBindVertexBuffer(2, vbo, offsetof(Vertex, color), sizeof(Vertex));
  }

};
*/

struct Vertex{

	Vertex(glm::vec3 p, glm::vec3 c){
		position[0] = p.x;
		position[1] = p.y;
		position[2] = p.z;
		color[0] = c.x;
		color[1] = c.y;
		color[2] = c.z;
	}

	float position[3];
	float color[3];

  static void format(int vbo){
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);

    glVertexAttribBinding(0, 0);
    glVertexAttribBinding(1, 1);

    glBindVertexBuffer(0, vbo, offsetof(Vertex, position), sizeof(Vertex));		//Internal Offset vs. Full Offset
    glBindVertexBuffer(1, vbo, offsetof(Vertex, color), sizeof(Vertex));
  }

};

/*
================================================================================
                              Render Pool Class
================================================================================
*/

//Drawing Command Struct

using namespace std;

struct DAIC {
  DAIC(){}
  DAIC(uint c, uint iC, uint s, uint bI){
    cnt = c; instCnt = iC; start = s; baseInst = bI;
  }
  uint cnt;
  uint instCnt;
  uint start;
  uint baseInst;
};

template<typename T>
class Renderpool {
private:

  GLuint vao;     //Vertex Array Object
  GLuint vbo;     //Vertex Buffer Object
  GLuint indbo;   //Indirect Draw Command Buffer Object

  size_t K = 0;   //Number of Reserved Vertices
  size_t N = 0;   //Number of Indirect Draw Calls
  bool fixedN = false;

  T* start;       //Immutable Storage Location Start
  list<T*> free;  //List of Free Data Addresses

public:

  vector<DAIC> indirect;  //Indirect Drawing Commands
  vector<int> indsize;    //Indirect Drawing Commands

  Renderpool(){
    glGenVertexArrays(1, &vao); //Buffer Generation
    glBindVertexArray(vao);
    glGenBuffers(1, &indbo);
    glGenBuffers(1, &vbo);
    T::format(vbo);
    N = 0;
    lock();
  }

  Renderpool(int k):Renderpool(){
    reserve(k);
  }

  Renderpool(int k, int n):Renderpool(k){
    N = n;
    fixedN = true;
    indirect.resize(N);
    indsize.resize(N);
  }

/*
================================================================================
                      Synchronization and Rendering
================================================================================
*/

private:

  GLsync gSync;

public:

  void lock(){
    if(gSync) glDeleteSync(gSync);
    gSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
  }

  void wait(){
  	if(gSync) while(true){
  			GLenum waitReturn = glClientWaitSync(gSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1 );
  			if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
  				return;
  	}
  }

  void render(const int first = 0, int length = 0, const GLenum mode = GL_TRIANGLES){

    if(length == 0)
      if(indirect.size() == 0) return;
      else length = indirect.size();

    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);

    wait();
    glMultiDrawArraysIndirect(mode, (void*)(first*sizeof(DAIC)), length, 0);
    lock();

  }

/*
================================================================================
                        VBO / Vertex Pool Sectioning
================================================================================
*/

// Generate an indirect draw command of length k, return the index

int section(int k, int ind = 0, bool active = true){
  if(k == 0) return 0;

  int n = 0;

  T* freestart = free.front();
  T* prev = freestart;
  for(T* next: free){

    if(next - prev == 0)
      continue;
    if(next - prev == 1){
      prev = next;
      n++;
    }
    else n = 0;
    if(n == k-1) break;
  }

  if(n < k -1){
    std::cout<<"Failed to Find Region"<<std::endl;
  }

  //Remove these specific guys from the list!
  typename list<T*>::iterator it1, it2;  //List of Free Data Addresses
  it1 = it2 = free.begin();
  advance(it1, prev-freestart-k+1);
  advance(it2, prev-freestart+1);
  free.erase(it1, it2);

  if(!fixedN){
    indirect.emplace_back(0, 1, prev-start-k+1, 0);
    indsize.push_back(k);
    ind = indirect.size()-1;
  }
  else{
    indirect[ind] = DAIC(0, 1, prev-start-k+1, 0);
    indsize[ind] = k;
  }

  if(active) activate(ind);
  else deactivate(ind);

  return ind;

}

// Remove the Indirect Draw Command, Free the Memory

void unsection(int& ind){
  N--;

  for(int k = indirect[ind].start; k < indirect[ind].start + indsize[ind]; k++)
    deallocate(start+k);
  free.sort();

  indirect.erase(indirect.begin()+ind);
  indsize.erase(indsize.begin()+ind);
  ind = 0;
  update();

}

// Activate / Deactivate Sections

void activate(int ind){
  indirect[ind].cnt = indsize[ind];
  update();
}

void deactivate(int ind){
  indirect[ind].cnt = 0;
  update();
}

// Upload Indirect Buffer to GPU

void update(){
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, indirect.size()*sizeof(DAIC), &indirect[0], GL_DYNAMIC_DRAW);
}

void simplify(){

  for(int i = 0; i < indirect.size()-1;){
    DAIC* cur = &indirect[i];
    DAIC* next = &indirect[i+1];

    if(cur->start + cur->cnt == next->start){
      cur->cnt += next->cnt;
      indirect.erase(indirect.begin()+i+1);
    }
    else i++;
  }

  //N = indirect.size();
  update();

}

/*
================================================================================
                            Raw Memory Pooling
================================================================================
*/

// Create Persistently Mapped Buffer for Memory Pooling

void reserve(int k){
  K = k;
  const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferStorage(GL_ARRAY_BUFFER, K*sizeof(T), NULL, flags);
  start = (T*)glMapBufferRange( GL_ARRAY_BUFFER, 0, K*sizeof(T), flags | GL_MAP_UNSYNCHRONIZED_BIT );
  for(size_t i = 0; i < K; i++)
    free.push_back(start+i);
}

template<typename... Args>
bool fill(int ind, int k, Args && ...args){
  assert(k < indsize[ind]);                           //In-Bound Check
  T* place = start + indirect[ind].start + k;         //Exact Location
  try{ new (place) T(std::forward<Args>(args)...); }  //Construct In-Place
  catch(...) { throw; return false; }
  return true;
}

void deallocate(T* o){      //Per-Address Deconstruction
  o->~T();
  free.push_back(o);
}

};

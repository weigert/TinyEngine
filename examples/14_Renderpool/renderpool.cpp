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
#include <set>

//Vertex Format

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
    cnt = c; instCnt = iC; start = s; baseInst = bI; baseCnt = c;
  }
  uint cnt;
  uint instCnt;
  uint start;
  uint baseInst;

	//Extra Properties
	uint baseCnt;					//Max Count
};

template<typename T>
class Renderpool {
private:

  GLuint vao;     //Vertex Array Object
  GLuint vbo;     //Vertex Buffer Object
  GLuint indbo;   //Indirect Draw Command Buffer Object

  size_t K;   		//Number of Reserved Vertices
  size_t N;   		//Number of Maximum Buckets

  T* start;       //Immutable Storage Location Start
	stack<T*> free;

	vector<DAIC> indirect;  //Indirect Drawing Commands

	Renderpool(){
    glGenVertexArrays(1, &vao); //Buffer Generation
    glBindVertexArray(vao);
    glGenBuffers(1, &indbo);
    glGenBuffers(1, &vbo);
    T::format(vbo);
    lock();
		K = 0; N = 0;
  }

public:

  Renderpool(int k, int n):Renderpool(){
		K = k; N = n;
    reserve(k, n);
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

		if(length > N) length = N;
    if(length == 0)
      if(indirect.size() == 0) return;
      else length = indirect.size();

    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);

    wait();
    glMultiDrawArraysIndirect(mode, (void*)(first*(sizeof(DAIC))), length, sizeof(DAIC));
    lock();

  }

/*
================================================================================
            VBO / Vertex Pool Sectioning / Activation / Updating
================================================================================
*/

// Generate an indirect draw command of length k, return the index

int section(const int k, const bool active = true){

  if(k == 0) return 0;
	if(k > K){
		std::cout<<"Requested "<<k<<" vertices for a bucket of size "<<K<<std::endl;
		return 0;
	}
	if(free.empty()){
		std::cout<<"No Buckets Available"<<std::endl;
		return 0;
	}

  indirect.emplace_back(k, 1, free.top()-start, 0);
	free.pop();

  const int ind = indirect.size()-1;
  if(!active) deactivate(ind);
	update();

	return ind;

}

// Remove the Indirect Draw Command, Free the Memory

void unsection(int first, int count = 1, int stride = 1){

	for(size_t i = first; i < first+count*stride; i++){
		free.push(start+indirect[i].start);
		for(int k = indirect[i].start; k < indirect[i].start + indirect[i].baseCnt; k++)
			deallocate(start+k);
	}

	for(size_t i = 0; i < count; i++)
		indirect.erase(indirect.begin()+first+stride-1); //Erasing makes the other indices no longer work...
	update();

}

void shrink(int first, int newsize){
	assert(newsize < K);
	indirect[first].baseCnt = newsize;
	indirect[first].cnt = newsize;
}

// Activate / Deactivate Sections (Strided)

void activate(int first, int count = 1, int stride = 1){

	if(count == 0) count = indirect.size();
	for(size_t i = first; first+i*stride < indirect.size() && i < count; i++)
		indirect[first+i*stride].cnt = indirect[first+i*stride].baseCnt;
  update();

}

void deactivate(int first, int count = 1, int stride = 1){

	if(count == 0) count = indirect.size();
	for(size_t i = 0; first+i*stride < indirect.size() && i < count; i++)
		indirect[first+i*stride].cnt = 0;
  update();

}

// Upload Indirect Buffer to GPU

void update(){

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, indirect.size()*sizeof(DAIC), &indirect[0], GL_DYNAMIC_DRAW);

}

/*
================================================================================
                            Raw Memory Pooling
================================================================================
*/

// Create Persistently Mapped Buffer for Memory Pooling

void reserve(int k, int n){
  const GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferStorage(GL_ARRAY_BUFFER, N*K*sizeof(T), NULL, flags);
  start = (T*)glMapBufferRange( GL_ARRAY_BUFFER, 0, N*K*sizeof(T), flags | GL_MAP_UNSYNCHRONIZED_BIT );
  for(size_t i = 0; i < N; i++)
		free.push(start+i*K);
}

template<typename... Args>
bool fill(const int ind, const int k, Args && ...args){
  assert(k < K); assert(ind < indirect.size());       //In-Bound Check
  T* place = start + indirect[ind].start + k;         //Exact Location
  try{ new (place) T(std::forward<Args>(args)...); }  //Construct In-Place
  catch(...) { throw; return false; }
  return true;
}

void deallocate(T* o){      //Per-Address Deconstruction
  o->~T();
}

};

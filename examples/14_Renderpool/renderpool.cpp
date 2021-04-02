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

		glVertexBindingDivisor(0, 0);
		glVertexBindingDivisor(1, 0);
		glVertexBindingDivisor(2, 0);

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
  DAIC(uint c, uint iC, uint s, uint bV, uint bI, uint g){
    cnt = c; instCnt = iC; start = s; baseVert = bV; baseInst = bI;
		baseCnt = c; group = g;
  }

  uint cnt;
  uint instCnt;
  uint start;
	uint baseVert;
  uint baseInst;

	//Extra Properties
	uint baseCnt;					//Max Count
	uint group;						//Group Membership
	glm::vec3 pos = glm::vec3(0);

};

#include <unordered_set>

template<typename T>
class Renderpool {
private:

  GLuint vao;     //Vertex Array Object
  GLuint vbo;     //Vertex Buffer Object
	GLuint ebo;			//Element Array Buffer Object
  GLuint indbo;   //Indirect Draw Command Buffer Object

  size_t K;   		//Number of Reserved Vertices
  size_t N;   		//Number of Maximum Buckets
	size_t M;				//Number of Active Buckets

  T* start;       //Immutable Storage Location Start
	stack<T*> free;

	vector<DAIC> indirect;  //Indirect Drawing Commands
	vector<DAIC> transfer;  //Indirect Drawing (Sorted)

	Renderpool(){
    glGenVertexArrays(1, &vao); //Buffer Generation
    glBindVertexArray(vao);
    glGenBuffers(1, &indbo);
    glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);//GL_ELEMENT_ARRAY_BUFFER
    T::format(vbo);
    lock();
		K = 0; N = 0; M = 0;
  }

public:

  Renderpool(int k, int n):Renderpool(){
		K = k; N = n; M = n;
		transfer.resize(n);
    reserve(k, n);
  }

	~Renderpool(){
		glBindVertexArray(vao);
		glDeleteBuffers(1, &ebo);
		glDeleteBuffers(1, &indbo);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
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
      else length = M;

    glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);

    wait();
		glMultiDrawElementsIndirect(mode, GL_UNSIGNED_INT, (void*)(first*(sizeof(DAIC))), length, sizeof(DAIC));
		lock();

  }

/*
================================================================================
            VBO / Vertex Pool Sectioning / Activation / Updating
================================================================================
*/

template<typename F, typename... Args>
void mask(F function, Args&&... args){

	M = 0;
	for(size_t i = 0; i < indirect.size(); i++)
		if(function(indirect[i], args...))
			transfer[M++] = indirect[i];

	sort(transfer.begin(), transfer.begin() + M, [](const DAIC& a, const DAIC& b){
		return (a.baseVert < b.baseVert);
		//return (length(cam::look + cam::pos - a.pos) < length(cam::look + cam::pos - b.pos));
	});

	update(false);

}

// Generate an indirect draw command of length k, return the index

int section(const int k, const int group = 0, const glm::vec3 pos = glm::vec3(0)){

  if(k == 0) return N;
	if(k > K) return N;
	if(free.empty()) return N;




	GLint first = 6*K;
	GLint base = (free.top()-start);
	free.pop();

	if(group%2 == 0) first = 0;






  indirect.emplace_back(k, 1, first, base, 0, group);
	indirect.back().pos = pos;

	transfer.push_back(indirect.back());

	const int ind = indirect.size()-1;

	return ind;

}

// Remove the Indirect Draw Command, Free the Memory

void unsection(int first, int count = 1, int stride = 1){

	for(size_t i = first; i < first+count*stride; i++){
		free.push(start+indirect[i].baseVert);
		for(int k = indirect[i].baseVert; k < K; k++)
			deallocate(start+k);
	}

	for(size_t i = 0; i < count; i++)
		indirect.erase(indirect.begin()+first+stride-1); //Erasing makes the other indices no longer work...

}

void shrink(int first, int newsize){

	assert(newsize < K);
	indirect[first].baseCnt = newsize;
	indirect[first].cnt = newsize;

}

// Activate / Deactivate Sections (Strided)

void activate(int first, int count = 1, int stride = 1){

	if(count == 0) count = indirect.size();
	for(size_t i = 0; first+i*stride < indirect.size() && i < count; i++)
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

void update(bool copy = true){

	if(copy) transfer = indirect;
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, transfer.size()*sizeof(DAIC), &transfer[0], GL_DYNAMIC_DRAW);

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
	for(int i = 0; i < N; i++)
		free.push(start+i*K);

	/*

		This is where we choose our indexing structures.
		Basically here I can say that there are 2 possible structures,
		and I have to pick the appropriate one based on the

	*/

	vector<GLuint> indices;

	for(size_t j = 0; j < K; j++){
		indices.push_back(j*4+0);
		indices.push_back(j*4+1);
		indices.push_back(j*4+2);
		indices.push_back(j*4+3);
		indices.push_back(j*4+1);
		indices.push_back(j*4+0);
	}
	for(size_t j = 0; j < K; j++){
		indices.push_back(j*4+0);
		indices.push_back(j*4+2);
		indices.push_back(j*4+1);
		indices.push_back(j*4+1);
		indices.push_back(j*4+3);
		indices.push_back(j*4+0);
	}





	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

}

template<typename... Args>
bool fill(const int ind, const int k, Args && ...args){
	if(k < k) std::cout<<k<<"/"<<K<<std::endl<<std::flush;
  assert(k < K); assert(ind < indirect.size());       //In-Bound Check
  T* place = start + indirect[ind].baseVert + k;         //Exact Location
  try{ new (place) T(std::forward<Args>(args)...); }  //Construct In-Place
  catch(...) { throw; return false; }
  return true;
}

void deallocate(T* o){      //Per-Address Deconstruction
  o->~T();
}

};

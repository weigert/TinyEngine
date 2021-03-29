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

  size_t K = 0;   //Number of Reserved Vertices
  size_t N = 0;   //Number of Indirect Draw Calls

  T* start;       //Immutable Storage Location Start

	vector<DAIC> indirect;  //Indirect Drawing Commands

	Renderpool(){
    glGenVertexArrays(1, &vao); //Buffer Generation
    glBindVertexArray(vao);
    glGenBuffers(1, &indbo);
    glGenBuffers(1, &vbo);
    T::format(vbo);
    lock();
  }

public:

	vector<T*> free;  	//List of Free Data Addresses
  Renderpool(int k):Renderpool(){
    reserve(k);
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

int section(int k, bool active = true){

  if(k == 0) return 0;

  int n = 0;
	int l = 0;
	int dist = 0;
  T* freestart = free[0];
  T* prev = freestart;

  for(T* next: free){
		l++;

    if(next - prev == dist) continue;


    if(next - prev == 1){
			n++;
			prev = next;
		}
    else{
			dist = next-prev;
			freestart = next;
			prev = next;
			n = 0;
			continue;
		}

    if(n == k) break;
  }

  if(n < k){
		std::cout<<free.size()<<std::endl;
    std::cout<<"Failed to Find Region"<<std::endl;
		std::cout<<"N: "<<n<<" K: "<<k<<std::endl;

		std::sort(free.begin(), free.end());

		n = 0;
		l = 0;
		dist = 0;
	  freestart = free[0];
	  prev = freestart;

	  for(T* next: free){
			l++;

	    if(next - prev == dist) continue;


	    if(next - prev == 1){
				n++;
				prev = next;
			}
	    else{
				dist = next-prev;
				freestart = next;
				prev = next;
				n = 0;
				continue;
			}

	    if(n == k) break;
	  }

		computefracture();

  }

	std::cout<<"Sectioning ";
	timer::benchmark<std::chrono::microseconds>([&](){

	free.erase(free.begin()+l-k-1, free.begin()+l-1);			//Remove Positions from Free

	});

  indirect.emplace_back(k, 1, freestart-start, 0);

  int ind = indirect.size()-1;
  if(!active) deactivate(ind);
	update();
	N++;

  return ind;

}

void computefracture(){
	int n = 0;
	int l = 0;
	T* freestart = free.front();
	T* prev = freestart;
	int dist = 0;
	for(T* next: free){

		if(next - prev == dist) continue;
		if(next - prev == 0x1){
			prev = next;
			n++;
		}
		else{
			n++;
	//		std::cout<<"SEGSIZE: "<<n<<std::endl;
			dist = next-prev;
			prev = next;
			n = 0;
			l++;
		}
	}

	std::cout<<"SEGS: "<<l<<std::endl;
}

// Remove the Indirect Draw Command, Free the Memory

void unsection(int first, int count = 1, int stride = 1){

	for(size_t i = first; i < first+count*stride; i++){
	  for(int k = indirect[i].start; k < indirect[i].start + indirect[i].baseCnt; k++)
	    deallocate(start+k);
	}

for(size_t i = 0; i < count; i++)
	indirect.erase(indirect.begin()+first+stride-1); //Erasing makes the other indices no longer work...
update();

//free.sort();

N--;

}

// Activate / Deactivate Sections (Strided)

void activate(int first, int stride = 0, int count = 0){
	if(count == 0) count = indirect.size();
	for(size_t i = 0; first+i*stride < indirect.size() && i < count; i++)
		indirect[first+i*stride].cnt = indirect[first+i*stride].baseCnt;
  update();
}

void deactivate(int start, int stride = 0, int count = 0){
	if(count == 0) count = indirect.size();
	for(size_t i = 0; start+i*stride < indirect.size() && i < count; i++)
		indirect[start+i*stride].cnt = 0;
  update();
}

// Upload Indirect Buffer to GPU

void update(){
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indbo);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, indirect.size()*sizeof(DAIC), &indirect[0], GL_DYNAMIC_DRAW);
}

/*
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

  N = indirect.size();
  update();

}
*/

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
  assert(k < indirect[ind].baseCnt);                           //In-Bound Check
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

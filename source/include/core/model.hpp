#ifndef TINYENGINE_CORE_MODEL
#define TINYENGINE_CORE_MODEL

#include <TinyEngine/Buffer>

#include <string>
#include <vector>
#include <unordered_map>

namespace Tiny {

//! A Model binds multiple Buffers into a single renderable form,
//! while informing OpenGL how to interpret the raw memory buffer.
//!
struct Model {

  //! Allocate GPU Model (Vertex Array Object)
  Model(){
    glGenVertexArrays(1, &vao);
  }

  //! Allocate GPU Model and Enable Buffer Binding Points
  Model(std::vector<std::string> nbinding):Model(){
    this->operator()();
    for(auto& b: nbinding){
      bindings[b] = bindings.size();
      glEnableVertexAttribArray(bindings[b]);
    }
  }

  //! De-Allocate GPU Model
  ~Model(){
    glDisableVertexAttribArray(vao);
    glDeleteVertexArrays(1, &vao);
    for(auto& buf: buffers)
      delete buf.second;
  }

  //! Activate the Model (Vertex Array Object)
  void operator()() const {
    glBindVertexArray(this->vao);
  }

  // Data Setting / Getting

  template<typename T> void bind(std::string binding, Buffer* buf, bool owned = false);
  void index(Buffer* buf, bool owned = false);
  void render(GLenum primitive = GL_TRIANGLE_STRIP);

  const size_t size() const {
    return _size;
  }

  Buffer* _index = NULL;
  std::unordered_map<std::string, int> bindings;        //Binding Points of Attributes
  std::unordered_map<std::string, Buffer*> buffers;  //Owned Buffer

protected:
  uint32_t vao;                                           //Vertex Array Object
  size_t _size = 0;                                      //Number of Vertices
};

template<typename T>
void Model::bind(std::string binding, Buffer* buf, bool owned){  //Bind a specific buffer to a binding point
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, buf->index());
  #ifdef TINYENGINE_OS_MAC
  glVertexAttribPointer( bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0, 0 );
  #else
  glBindVertexBuffer(bindings[binding], buf->index(), 0, sizeof(T));
  glVertexAttribFormat(bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0);
  #endif
  if(owned) buffers[binding] = buf;
}

void Model::index(Buffer* buf, bool owned){
  _index = buf;
  _size = _index->size();///sizeof(int);
  if(owned) buffers["index"] = _index;
}

void Model::render(GLenum mode){
  glBindVertexArray(vao);
  if(_index != NULL){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index->index());
    glDrawElements(mode, _size, GL_UNSIGNED_INT, 0);
  }
  else glDrawArrays(mode, 0, _size);
}

// Model-Type Instantiations

struct Point: Model {
private:
  Buffer vert;
public:
  Point():
    Model({"vert"}),
    vert(std::vector<glm::vec3>{
      {0.0f, 0.0f, 0.0f}
    }){
    _size = 1;
    bind<glm::vec3>("vert", &vert);
  }
};

struct Square2D: Model {
private:
  Buffer vert, tex;
public:
  Square2D():
    Model({"in_Quad", "in_Tex"}),
    vert(std::vector<glm::vec2>{
      {-1.0f, -1.0f},
      { 1.0f, -1.0f},
      {-1.0f,  1.0f},
      { 1.0f,  1.0f}
    }),
    tex(std::vector<glm::vec2>{
      { 0.0f,  1.0f},
      { 1.0f,  1.0f},
      { 0.0f,  0.0f},
      { 1.0f,  0.0f}
    }){
    _size = 4;
    bind<glm::vec2>("in_Quad", &vert);
    bind<glm::vec2>("in_Tex", &tex);
  }
};

struct Square3D: Model {
private:
  Buffer vert;
  Buffer tex;
public:
  Square3D():
    Model({"in_Quad", "in_Tex"}),
    vert(std::vector<glm::vec3>{
      {-1.0f,  1.0f,  0.0f},
      {-1.0f, -1.0f,  0.0f},
      { 1.0f,  1.0f,  0.0f},
      { 1.0f, -1.0f,  0.0f}
    }),
    tex(std::vector<glm::vec2>{
      { 0.0f,  0.0f},
      { 0.0f,  1.0f},
      { 1.0f,  0.0f},
      { 1.0f,  1.0f}
    }){
    _size = 4;
    bind<glm::vec3>("in_Quad", &vert);
    bind<glm::vec2>("in_Tex", &tex);
  }
};

struct Gizmo: Model {
private:
  Buffer vert, tex;
public:
  Gizmo():
    Model({"in_Quad", "in_Tex"}),
    vert(std::vector<glm::vec3>{
      { 0.0f, 0.0f, 0.0f},
      { 1.0f, 0.0f, 0.0f},
      { 0.0f, 0.0f, 0.0f},
      { 0.0f, 1.0f, 0.0f},
      { 0.0f, 0.0f, 0.0f},
      { 0.0f, 0.0f, 1.0f}
    }),
    tex(std::vector<glm::vec3>{
      { 1.0f, 0.0f, 0.0f}, 
      { 1.0f, 0.0f, 0.0f},
      { 0.0f, 1.0f, 0.0f}, 
      { 0.0f, 1.0f, 0.0f},
      { 0.0f, 0.0f, 1.0f}, 
      { 0.0f, 0.0f, 1.0f}
    }){
    _size = 6;
    bind<glm::vec3>("in_Quad", &vert);
    bind<glm::vec3>("in_Tex", &tex);
  }
};

struct Triangle: Model {
private:
	Buffer vert;
public:
	Triangle():
    Model({"vert"}),
    vert(std::vector<glm::vec4>{
      {1.0f, 0.0f, 0.0f, 0.0f},
		  {0.0f, 1.0f, 0.0f, 0.0f},
		  {0.0f, 0.0f, 1.0f, 0.0f},
		  {1.0f, 0.0f, 0.0f, 0.0f}
    }){
    _size = 4;
		bind<glm::vec4>("vert", &vert);
	}
};

struct Cube: Model {
private:
  Buffer vert;
  Buffer tex;
public:
  Cube():
    Model({"in_Quad", "in_Tex"}),
    vert(std::vector<glm::vec3>{
      { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, // front
      {-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, // back
      {-1.0f, -1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f}, // left
      { 1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, // right
      {-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f,  1.0f}, // top
      { 1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}  // bottom
    }),
  tex(std::vector<glm::vec2>{
    {0.0f,  0.0f},
    {0.0f,  1.0f},
    {1.0f,  0.0f},
    {1.0f,  1.0f}
  }){
    _size = 36;
    bind<glm::vec3>("in_Quad", &vert);
    bind<glm::vec2>("in_Tex", &tex);
  }
};

} // end of namespace Tiny

#endif
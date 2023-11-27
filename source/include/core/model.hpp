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
    glGenVertexArrays(1, &_vao);
  }

  //! De-Allocate GPU Model
  ~Model(){
    glDisableVertexAttribArray(_vao);
    glDeleteVertexArrays(1, &_vao);
  }

  //! Allocate GPU Model and Enable Buffer Binding Points
  Model(std::vector<std::string> nbinding):Model(){
    this->operator()();
    for(auto& b: nbinding){
      bindings[b] = bindings.size();
      glEnableVertexAttribArray(bindings[b]);
    }
  }

  // Data Setting / Getting

  template<typename T> void bind(std::string binding, Buffer& buf);
  void render(const GLenum, const size_t) const;  //!< Render the Instance w. Primitive, Number of Instances
  void render() const;                            //!< Default Primitive, Size

  inline const uint32_t vao() const { return _vao; }            //!< Retrieve Vertex Array Object Index
  inline const size_t size()  const { return _size; }           //!< Retrieve Number of Vertices
  inline const size_t count() const { return bindings.size(); } //!< Retrieve Binding Count
  void operator()() const { glBindVertexArray(this->vao()); }   //!< Bind the Vertex Array Object

protected:
  uint32_t _vao;                                  //!< Vertex Array Object Index
  size_t _size;                                   //!< Number of Vertices
  std::unordered_map<std::string, int> bindings;  //!< Binding Count
};

template<typename T>
void Model::bind(std::string binding, Buffer& buf){  //Bind a specific buffer to a binding point
  this->operator()();
  buf.operator()();
  _size = buf.size<T>();
  #ifdef TINYENGINE_OS_MAC
  glVertexAttribPointer(bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0, 0 );
  #else
  glBindVertexBuffer(bindings[binding], buf.index(), 0, sizeof(T));
  glVertexAttribFormat(bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0);
  #endif
}

void Model::render(const GLenum primitive, const size_t size) const {
  this->operator()();
  glDrawArrays(primitive, 0, size);
}

void Model::render() const {
  render(GL_TRIANGLE_STRIP, this->size());
}

// Indexed Model

struct Indexed: Model {

  template<typename T>
  Indexed(Buffer& _index):Model(),
    _size(_index.size()/sizeof(T)),
    _index(_index){}

  void render(const GLenum primitive) const {
    _index(GL_ELEMENT_ARRAY_BUFFER);
    glDrawElements(primitive, _size, GL_UNSIGNED_INT, 0);
  }

  inline Buffer& index() const      { return _index; }
  inline const size_t size() const  { return _size; }

private:
  Buffer& _index;
  size_t _size;
};

// Model-Type Instantiations

struct Point: Model {
private:
  Buffer vert;
public:
  Point():Model({"vert"}),
    vert(std::vector<glm::vec3>{
      {0.0f, 0.0f, 0.0f}
    }){
    _size = 1;
    bind<glm::vec3>("vert", vert);
  }
};

struct Square2D: Model {
private:
  Buffer vert, tex;
public:
  Square2D():Model({"in_Quad", "in_Tex"}),
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
    bind<glm::vec2>("in_Quad", vert);
    bind<glm::vec2>("in_Tex", tex);
  }
};

struct Square3D: Model {
private:
  Buffer vert;
  Buffer tex;
public:
  Square3D():Model({"in_Quad", "in_Tex"}),
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
    bind<glm::vec3>("in_Quad", vert);
    bind<glm::vec2>("in_Tex", tex);
  }
};

struct Gizmo: Model {
private:
  Buffer vert, tex;
public:
  Gizmo():Model({"in_Quad", "in_Tex"}),
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
    bind<glm::vec3>("in_Quad", vert);
    bind<glm::vec3>("in_Tex", tex);
  }
};

struct Triangle: Model {
private:
	Buffer vert;
public:
	Triangle():Model({"vert"}),
    vert(std::vector<glm::vec4>{
      {1.0f, 0.0f, 0.0f, 0.0f},
		  {0.0f, 1.0f, 0.0f, 0.0f},
		  {0.0f, 0.0f, 1.0f, 0.0f},
		  {1.0f, 0.0f, 0.0f, 0.0f}
    }){
    _size = 4;
		bind<glm::vec4>("vert", vert);
	}
};

struct Cube: Model {
private:
  Buffer vert;
  Buffer tex;
public:
  Cube():Model({"in_Quad", "in_Tex"}),
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
    bind<glm::vec3>("in_Quad", vert);
    bind<glm::vec2>("in_Tex", tex);
  }
};

} // end of namespace Tiny

#endif
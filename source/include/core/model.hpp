#ifndef TINYENGINE_CORE_MODEL
#define TINYENGINE_CORE_MODEL

#include <TinyEngine/Buffer>

#include <string>
#include <vector>
#include <unordered_map>

namespace Tiny {

//! A Model binds multiple Buffers to named Attributes, to make them
//! jointly renderable as the vertices of a primitive.
//!
//! The named Attributes of the Model are declared at construction time.
//! A single Buffer can then be bound to each named Attribute, by
//! declaring how the data in the buffer should be interpreted.
//! 
//! When a Model is rendered as a primitive type, the bound buffers
//! are assumed to be in full array form. If you want your primitive
//! to utilized an indexed draw / elements array, then utilized the
//! indexed derived class.
//!
struct Model {
protected:
  //! Allocate GPU Model (Vertex Array Object)
  Model(){
    glGenVertexArrays(1, &_vao);
  }

public:
  //! De-Allocate GPU Model
  ~Model(){
    glDisableVertexAttribArray(_vao);
    glDeleteVertexArrays(1, &_vao);
  }

  //! Allocate GPU Model and Enable Buffer Binding Points
  Model(const std::vector<std::string> attributes):Model(){
    this->operator()();
    for(auto& attr: attributes){
      this->attributes[attr] = this->attributes.size();
      glEnableVertexAttribArray(this->attributes[attr]);
    }
  }

  //! Bind a Buffer to a named Attribute, while declaring how
  //! the data should be interpreted for each primitive.
  //!
  template<typename T> void bind(const std::string attribute, Buffer& buffer);
  
  void render(const GLenum, const size_t) const;  //!< Render the Model as Primitive, Number of Primitives
  void render() const;                            //!< Default Primitive, Size (Full Buffer)

  inline const uint32_t vao() const { return _vao; }              //!< Retrieve Vertex Array Object Index
  inline const size_t size()  const { return _size; }             //!< Retrieve Number of Vertices
  inline const size_t count() const { return attributes.size(); } //!< Retrieve Binding Count
  void operator()() const { glBindVertexArray(this->vao()); }     //!< Bind the Vertex Array Object

protected:
  uint32_t _vao;                                    //!< Vertex Array Object Index
  size_t _size;                                     //!< Number of Vertices
  std::unordered_map<std::string, int> attributes;  //!< Local Attribute Map
};

template<typename T>
void Model::bind(const std::string attribute, Buffer& buffer){
  this->operator()();                             // Bind Model
  buffer.operator()();                            // Bind Buffer
  _size = buffer.size<T>();                       // Update Model Size
  const int index = this->attributes[attribute];  // Attribute Binding Index
  #ifdef TINYENGINE_OS_MAC
  glVertexAttribPointer(index, sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0, 0 );
  #else
  glBindVertexBuffer(index, buffer.index(), 0, sizeof(T));
  glVertexAttribFormat(index, sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0);
  #endif
}

void Model::render(const GLenum primitive, const size_t size) const {
  this->operator()();
  glDrawArrays(primitive, 0, size);
}

void Model::render() const {
  this->render(GL_TRIANGLE_STRIP, this->size());
}

// Indexed Model

//! Indexed is a Model with an additional index buffer, which is used
//! as the element array buffer when drawing primitives.
//!
//! The element array buffer maps the vertices of a primitive
//! to the elements in the attribute buffers.
//!
struct Indexed: Model {
protected:
  using Model::Model;

public:
  Indexed(const std::vector<std::string> attributes, Buffer& _index):
    _index(_index),
    Model(attributes){
      set(_index);
    }

  //! Set the index Buffer with an index type
  template<typename T = int>
  void set(Buffer& _index){
    this->_index = _index;
    _size = this->_index.size<T>();
  }

  void render(const GLenum, const size_t) const;  //!< Render as Primitive, Manual Size
  void render(const GLenum) const;                //!< Render as Primitive, Size is Index Buffer Size

  inline const Buffer& index() const  { return _index; }  //!< Retrieve the Index Buffer
  inline const size_t size()   const  { return _size; }   //!< Retrieve the Index Buffer Size

protected:
  Buffer& _index; //!< Index Buffer Reference
  size_t _size;   //!< Index Buffer Size
};

void Indexed::render(const GLenum primitive, const size_t size) const {
  this->operator()();               // Bind the VAO
  _index(GL_ELEMENT_ARRAY_BUFFER);  // Bind the Index Buffer
  glDrawElements(primitive, size, GL_UNSIGNED_INT, 0);
}

void Indexed::render(const GLenum primitive) const {
  this->render(primitive, this->size());
}

// Model-Type Instantiations

//! Point: Model({"vert"}):
//!   Single 3D Vertex at (0, 0, 0)
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

//! Square2D: Model({"in_Quad", "in_Tex"})
//!   4x2D Vertices at Corners of [-1, 1]
//!   4x2D UV-Coords at Corners of [0, 1] 
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

//! Square3D: Model({"in_Quad", "in_Tex"})
//!   4x3D Vertices at Corners of [-1, 1], Z = 0
//!   4x2D UV-Coords at Corners of [0, 1]
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

//! Gizmo: Model({"in_Quad", "in_Tex"})
//!   6x3D Vertices = 3x3D Line from Origin to Unit-Vector
//!   6xRGB Color-Values (RRGGBB)
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

//! Triangle is a model which contains 4 raw vertices of unit-vectors, forming a loop
//! The purpose of the unit-vectors is to be able to sub-index another buffer (e.g. SSBO)
//! for getting the actual coordinates of the vertices of the triangle when instance
//! rendering this primitive. This is not intended to be rendered directly.
//!
//! Example: Buffer A = TriangleIndices, Buffer B = TriangleVertices
//!   -> Instance Render the Triangle Model
//!   -> Utilize T.x, T.y, T.z + glInstanceId to Index Buffer A
//!   -> Utilize Indexed Buffer A to Lookup buffer B
//!   -> Emit Vertex Position from Vertex shader
//!
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

//! Cube: Model{"in_Quad", "in_Tex"}
//!   Unit-Cube Mesh on Domain ([-1, 1], [-1, 1], [-1, 1])
//!
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
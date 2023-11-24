#ifndef TINYENGINE_CORE_MODEL
#define TINYENGINE_CORE_MODEL

#include <TinyEngine/Buffer>

#include <string>
#include <vector>
#include <unordered_map>

namespace Tiny {

struct Model {

  GLuint vao;                                       //Vertex Array Object
  size_t SIZE = 0;                                  //Number of Vertices

  std::unordered_map<std::string, int> bindings;        //Binding Points of Attributes
  std::unordered_map<std::string, BufferRef*> buffers;  //Owned Buffer

  Model(){
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  }

  ~Model(){
    glDisableVertexAttribArray(vao);
    glDeleteVertexArrays(1, &vao);
    for(auto& buf: buffers)    //Delete Owned Buffers
      delete buf.second;
  }

  Model(std::vector<std::string> nbinding):Model(){     //Generate Model with Defined Bindings
    for(auto& b: nbinding){
      bindings[b] = bindings.size();          //Give the Named Binding Point an Index
      glEnableVertexAttribArray(bindings[b]); //Enable the Binding Point
    }
  }

  template<typename T>
  void bind(std::string binding, Buffer<T>* buf, bool owned = false){  //Bind a specific buffer to a binding point
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buf->index);
    #ifdef TINYENGINE_OS_MAC
    glVertexAttribPointer( bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0, 0 );
    #else
    glBindVertexBuffer(bindings[binding], buf->index, 0, sizeof(T));
    glVertexAttribFormat(bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0);
    #endif
    if(owned) buffers[binding] = buf;
  }

  GLuint iindex;
  bool indexed = false;
  void index(BufferRef* buf, bool owned = false){
    indexed = true;
    SIZE = buf->SIZE;
    iindex = buf->index;
    if(owned) buffers["index"] = buf;
  }

  glm::mat4 model = glm::mat4(1.0f);                //Model Matrix

  void render(GLenum mode = GL_TRIANGLE_STRIP){
    glBindVertexArray(vao);
    if(indexed){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iindex);
      glDrawElements(mode, SIZE, GL_UNSIGNED_INT, 0);
    }
    else glDrawArrays(mode, 0, SIZE);
  }

};

// Model-Type Instantiations

struct Point: Model {
private:
  Buffer<glm::vec3> vert;
public:
  Point():
    Model({"vert"}),
    vert({
      {0.0f, 0.0f, 0.0f}
    }){
    SIZE = 1;
    bind("vert", &vert);
  }
};

struct Square2D: Model {
private:
  Buffer<glm::vec2> vert, tex;                                      //2 Buffers
public:
  Square2D():
    Model({"in_Quad", "in_Tex"}),                          //2 Binding Points
    vert({ //Initialize Buffer "vert"
      {-1.0f, -1.0f},
      { 1.0f, -1.0f},
      {-1.0f,  1.0f},
      { 1.0f,  1.0f}
    }),
    tex({ //Initialize Buffer "tex"
      { 0.0f,  1.0f},
      { 1.0f,  1.0f},
      { 0.0f,  0.0f},
      { 1.0f,  0.0f}
    }){
    SIZE = 4;
    bind("in_Quad", &vert);                                       //Bind vert as glm::vec2
    bind("in_Tex", &tex);                                         //Bind tex as glm::vec2
  }
};

struct Square3D: Model {
private:
  Buffer<glm::vec3> vert;
  Buffer<glm::vec2> tex;
public:
  Square3D():
    Model({"in_Quad", "in_Tex"}),
    vert({
      {-1.0f,  1.0f,  0.0f},
      {-1.0f, -1.0f,  0.0f},
      { 1.0f,  1.0f,  0.0f},
      { 1.0f, -1.0f,  0.0f}
    }),
    tex({
      { 0.0f,  0.0f},
      { 0.0f,  1.0f},
      { 1.0f,  0.0f},
      { 1.0f,  1.0f}
    }){
    SIZE = 4;
    bind("in_Quad", &vert);
    bind("in_Tex", &tex);
  }
};

struct Gizmo: Model {
private:
  Buffer<glm::vec3> vert, tex;
public:
  Gizmo():
    Model({"in_Quad", "in_Tex"}),
    vert({
      { 0.0f, 0.0f, 0.0f},
      { 1.0f, 0.0f, 0.0f},
      { 0.0f, 0.0f, 0.0f},
      { 0.0f, 1.0f, 0.0f},
      { 0.0f, 0.0f, 0.0f},
      { 0.0f, 0.0f, 1.0f}
    }),
    tex({
      { 1.0f, 0.0f, 0.0f}, 
      { 1.0f, 0.0f, 0.0f},
      { 0.0f, 1.0f, 0.0f}, 
      { 0.0f, 1.0f, 0.0f},
      { 0.0f, 0.0f, 1.0f}, 
      { 0.0f, 0.0f, 1.0f}
    }){
    SIZE = 6;
    bind("in_Quad", &vert);
    bind("in_Tex", &tex);
  }
};

struct Triangle: Model {
private:
	Buffer<glm::vec4> vert;
public:
	Triangle():
    Model({"vert"}),
    vert({
      {1.0f, 0.0f, 0.0f, 0.0f},
		  {0.0f, 1.0f, 0.0f, 0.0f},
		  {0.0f, 0.0f, 1.0f, 0.0f},
		  {1.0f, 0.0f, 0.0f, 0.0f}
    }){
    SIZE = 4;
		bind("vert", &vert);
	}
};

struct Cube: Model {
private:
  Buffer<glm::vec3> vert;
  Buffer<glm::vec2> tex;
public:
  Cube():
    Model({"in_Quad", "in_Tex"}),
    vert({
      { 1.0f, -1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, // front
      {-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, // back
      {-1.0f, -1.0f,  1.0f}, {-1.0f,  1.0f,  1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f,  1.0f}, // left
      { 1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, // right
      {-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f, -1.0f}, {-1.0f,  1.0f,  1.0f}, // top
      { 1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f,  1.0f}, {-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f}  // bottom
    }),
  tex({
    {0.0f,  0.0f},
    {0.0f,  1.0f},
    {1.0f,  0.0f},
    {1.0f,  1.0f}
  }){
    SIZE = 36;
    bind("in_Quad", &vert);
    bind("in_Tex", &tex);
  }
};

} // end of namespace Tiny

#endif
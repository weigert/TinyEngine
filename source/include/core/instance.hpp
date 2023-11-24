#ifndef TINYENGINE_CORE_INSTANCE
#define TINYENGINE_CORE_INSTANCE

#include <TinyEngine/Buffer>
#include <TinyEngine/Model>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Tiny {

struct Instance {

  size_t SIZE;                                    //Number of Instances
  Instance(Model* m):model(m){};

  template<typename T> void bind(std::string, Buffer<T>*);
  template<typename T> void config(Buffer<T>*);
  void render(GLenum mode = GL_TRIANGLE_STRIP);
  void render(GLenum mode, int size);

private:
  Model* model;                                   //Model Pointer
  std::unordered_map<std::string, int> instances; //Binding Points of Attributes
};

template<typename T>
void Instance::bind(std::string name, Buffer<T>* buf){
  SIZE = buf->SIZE;
  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, buf->index);
  config(buf);
  instances[name] = buf->index;
}

template<typename T>
void Instance::config(Buffer<T>* buf){
  glEnableVertexAttribArray(model->bindings.size()+instances.size());
  glVertexAttribPointer(model->bindings.size()+instances.size(), sizeof(T)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribDivisor(model->bindings.size()+instances.size(), 1);
}

void Instance::render(GLenum mode, int size){
  glBindVertexArray(model->vao);
  if(model->indexed){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->iindex);
    glDrawElementsInstanced(mode, model->SIZE, GL_UNSIGNED_INT, 0, size);
  }
  else glDrawArraysInstanced(mode, 0, model->SIZE, size);
}

template<> //For Matrices - Special Procedure
void Instance::config<glm::mat4>(Buffer<glm::mat4>* buf){
  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(model->bindings.size()+instances.size()+i);
    glVertexAttribPointer(model->bindings.size()+instances.size()+i, 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(model->bindings.size()+instances.size()+i, 1);
  }
}

void Instance::render(GLenum mode){
  render(mode, SIZE);
}

} // end of namespace Tiny

#endif

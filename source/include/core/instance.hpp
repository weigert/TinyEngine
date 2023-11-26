#ifndef TINYENGINE_CORE_INSTANCE
#define TINYENGINE_CORE_INSTANCE

#include <TinyEngine/Buffer>
#include <TinyEngine/Model>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Tiny {

//! An Instance is a combination of a Model (Reference) and an Attribute Buffer, which
//! allow for the instanced rendering of the Model, once per Attribute Buffer-Element.
//!
struct Instance {

  //! Construct Instance with Model Reference
  Instance(Model& m):
    _model(m){};

  template<typename T> void bind(const std::string, const Buffer&);   //!< Instantiate the Model for every Buffer Component
  template<typename T> void config(const Buffer&);                    //!< Setup Attribute Divisor
  
  void render(const GLenum primitive, const size_t N) const;          //!< Render the Instance w. Primitive, Number of Instances
  void render() const;                                                //!< Default Primitive, Size

  const inline Model& model() const { return _model; }  //!< Retrieve the Model Reference
  const inline size_t count() const { return _count; }  //!< Retrieve Number of Attributes
  const inline size_t size()  const { return _size; }   //!< Retrieve Number of Attribute Elements

private:
  Model& _model;  //!< Model Reference
  size_t _count;  //!< Number of Attribute Divisors
  size_t _size;   //!< Default Number of Instances
};

// Attribute Divisor Binding / Configuration

template<typename T>
void Instance::bind(const std::string name, const Buffer& _buf){
  _model.operator()();    // Activate Model
  _buf.operator()();      // Activate Buffer
  config<T>(_buf);        // Configure Buffer Divisor
  _size = _buf.size<T>(); // Size of Buffer in T = N Instances
  _count = _count+1;      // Increment Number of Attributes
}

template<typename T>
void Instance::config(const Buffer& buf){
  glEnableVertexAttribArray(_model.bindings.size() + _count);
  glVertexAttribPointer(_model.bindings.size() + _count, sizeof(T)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribDivisor(_model.bindings.size() + _count, 1);
}

template<>
void Instance::config<glm::mat4>(const Buffer& buf){
  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(_model.bindings.size() + _count + i);
    glVertexAttribPointer(_model.bindings.size() + _count + i, 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(_model.bindings.size() + _count + i, 1);
  }
}

// Dispatch Render Call

void Instance::render(const GLenum primitive, const size_t size) const {
  _model.operator()();
  if(_model._index != NULL){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model._index->index());
    glDrawElementsInstanced(primitive, _model.size(), GL_UNSIGNED_INT, 0, size);
  }
  else glDrawArraysInstanced(primitive, 0, _model.size(), size);
}

void Instance::render() const {
  render(GL_TRIANGLE_STRIP, _size);
}

} // end of namespace Tiny

#endif

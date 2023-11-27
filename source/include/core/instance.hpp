#ifndef TINYENGINE_CORE_INSTANCE
#define TINYENGINE_CORE_INSTANCE

#include <TinyEngine/Buffer>
#include <TinyEngine/Model>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Tiny {

namespace {

template<typename T> void config(const size_t index){
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, sizeof(T)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribDivisor(index, 1);
}

template<> void config<glm::mat4>(const size_t index){
  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(index + i);
    glVertexAttribPointer(index + i, 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(index + i, 1);
  }
}

}

//! An Instance is a combination of a Model (Reference) and an Attribute Buffer, which
//! allow for the instanced rendering of the Model, once per Attribute Buffer-Element.
//!
template<typename M = Tiny::Model>
struct Instance {

  //! Construct Instance with Model Reference
  Instance(M& m):
    _model(m){};

  // Data Setting / Getting

  template<typename T> void bind(const Buffer&);  //!< Instantiate the Model for every Buffer Component
  void render(const GLenum, const size_t) const;  //!< Render the Instance w. Primitive, Number of Instances
  void render() const;                            //!< Default Primitive, Size

  const inline M& model() const { return _model; }      //!< Retrieve the Model Reference
  const inline size_t count() const { return _count; }  //!< Retrieve Number of Attribute Divisors
  const inline size_t size()  const { return _size; }   //!< Retrieve Number of Attribute Elements

private:
  M& _model;      //!< Model Reference
  size_t _size;   //!< Number of Attribute Elements
  size_t _count;  //!< Number of Attribute Divisors
};

// Attribute Divisor Binding / Configuration

template<typename M>
template<typename T>
void Instance<M>::bind(const Buffer& _buf){
  _model();                           // Activate Model
  _buf();                             // Activate Buffer
  config<T>(_model.count() + _count); // Configure Buffer Divisor
  _size = _buf.size<T>();             // Size of Buffer in T = N Instances
  _count = _count+1;                  // Increment Number of Attributes
}

// Dispatch Render Call

template<typename M>
void Instance<M>::render(const GLenum primitive, const size_t N) const {
  _model.operator()();
  glDrawArraysInstanced(primitive, 0, _model.size(), N);
}

template<>
void Instance<Tiny::Indexed>::render(const GLenum primitive, const size_t N) const {
  _model.operator()();
  _model.index()(GL_ELEMENT_ARRAY_BUFFER);
  glDrawElementsInstanced(primitive, _model.size(), GL_UNSIGNED_INT, 0, N);
}

template<typename M>
void Instance<M>::render() const {
  render(GL_TRIANGLE_STRIP, _size);
}

} // end of namespace Tiny

#endif

#ifndef TINYENGINE_CORE_BUFFER
#define TINYENGINE_CORE_BUFFER

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace Tiny {

//! \brief A Buffer is an opaque OpenGL array buffer object reference
//! 
//! Construction and Destruction of the Buffer will allocate
//! and de-allocate the GPU enabled buffer.
//!
//! Data can be uploaded to and retrieved from the GPU through
//! the Buffer type, using templated member functions.
//!
//! Buffers are filled and retrieved from raw memory pointers.
//!
struct Buffer {

  Buffer(){ glGenBuffers(1, &_index); }     //!< Allocate GPU Buffer
  ~Buffer(){ glDeleteBuffers(1, &_index); } //!< De-Allocate GPU Buffer

  //! Allocate GPU Buffer, Fill w. Data from Vector
  template<typename T>
  Buffer(const std::vector<T>& buf):Buffer(){
    set(buf);
  }

  //! Allocate GPU Buffer, Fill w. Data from Raw Memory Pointer
  template<typename T>
  Buffer(const size_t size, const T* data):Buffer(){
    set(size, data);
  }

  template<typename T> void set(const size_t size, const T* data);  //!< Set Data from Raw Memory
  template<typename T> void set(const std::vector<T>& buf);         //!< Set Data from Vector

  template<typename T> void get(const size_t size, T* data) const;  //!< Get Data into Raw Memory
  template<typename T> void get(std::vector<T>& buf) const;         //!< Get Data into Vector (Note: Must be Reserved!)

  template<typename T = GLfloat>
  const inline size_t size()    const { return _size/sizeof(T); }   //!< Return the Buffer Size in T
  const inline uint32_t index() const { return _index; }            //!< Get the OpenGL Buffer Pointer
  void operator()(const GLenum E = GL_ARRAY_BUFFER) const {         //!< Bind the Buffer
    glBindBuffer(E, this->index());
  }

private:
  uint32_t _index;  //!< Underlying OpenGL Buffer Index
  size_t _size;     //!< Size of the Buffer in Bytes
};

template<typename T>
void Buffer::set(const size_t size, const T* data){
  this->operator()();                                           // Bind this Buffer
  _size = size*sizeof(T);                                       // Set Buffer Size to Number of Bytes
  glBufferData(GL_ARRAY_BUFFER, _size, data, GL_STATIC_DRAW);   // Fill the Buffer w. Raw Memory Data
  glBindBuffer(GL_ARRAY_BUFFER, 0);                             // Un-Bind this Buffer
}

template<typename T>
void Buffer::set(const std::vector<T>& buf){
  set(buf.size(), &buf[0]);
}

template<typename T>
void Buffer::get(const size_t size, T* data) const {
  this->operator()();                                           // Bind this Buffer
  glGetBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(T), data); // Load the Data to Pointer
  glBindBuffer(GL_ARRAY_BUFFER, 0);                             // Un-Bind this Buffer
}

template<typename T>
void Buffer::get(std::vector<T>& buf) const {
  get(buf.size(), &buf[0]);
}

} // end of namespace Tiny

#endif
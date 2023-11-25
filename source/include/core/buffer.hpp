#ifndef TINYENGINE_CORE_BUFFER
#define TINYENGINE_CORE_BUFFER

#include <cstddef>
#include <glm/glm.hpp>
#include <vector>

namespace Tiny {

//! Buffer is an opaque OpenGL Array Buffer Object Reference
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

  //! Allocate GPU Buffer
  Buffer(){ 
    glGenBuffers(1, &_index);
  }

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

  //! De-Allocate GPU Buffer
  ~Buffer(){
    glDeleteBuffers(1, &_index);
  }

  // Data Setting / Getting
  template<typename T> void set(const size_t size, const T* data);  //!< Set Data from Raw Memory
  template<typename T> void set(const std::vector<T>& buf);         //!< Set Data from Vector
  template<typename T> void set(const T val);                       //!< Set Data from Single Value

  template<typename T> void get(const size_t size, T* data) const;  //!< Get Data into Raw Memory
  template<typename T> void get(std::vector<T>& buf) const;         //!< Get Data into Vector (Note: Must be Reserved!)
  template<typename T> void get(T& val) const;                      //!< Get Data into Single Value

  // Data Inspection

  const uint32_t index() const {  //!< Get the OpenGL Buffer Pointer
    return _index;
  }

  const size_t size() const {     //!< Return the Buffer Size in Bytes
    return _size;
  }

private:
  uint32_t _index;  //!< Underlying OpenGL Buffer Index
  size_t _size;     //!< Size of the Buffer in Bytes
};

template<typename T>
void Buffer::set(const size_t size, const T* data){
  _size = size*sizeof(T);                                       // Set Buffer Size to Number of Bytes
  glBindBuffer(GL_ARRAY_BUFFER, _index);                        // Bind the Buffer Index (Make Active)
  glBufferData(GL_ARRAY_BUFFER, _size, data, GL_DYNAMIC_DRAW);  // Fill the Buffer w. Raw Memory Data
  glBindBuffer(GL_ARRAY_BUFFER, 0);                             // Un-Bind the Buffer Index
}

template<typename T>
void Buffer::set(const std::vector<T>& buf){
  set(buf.size(), &buf[0]);
}

template<typename T>
void Buffer::set(const T val){
  set(1, &val);
}

template<typename T>
void Buffer::get(const size_t size, T* data) const {
  glBindBuffer(GL_ARRAY_BUFFER, _index);
  glGetBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(T), data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Buffer::get(std::vector<T>& buf) const {
  get(buf.size(), &buf[0]);
}

template<typename T>
void Buffer::get(T& val) const {
  get(1, &val);
}

} // end of namespace Tiny

#endif
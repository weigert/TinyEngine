#ifndef TINYENGINE_CORE_BUFFER
#define TINYENGINE_CORE_BUFFER

#include <cstddef>
#include <glm/glm.hpp>
#include <vector>

namespace Tiny {

struct BufferRef {

  BufferRef(){ 
    glGenBuffers(1, &index);
  }

  ~BufferRef(){
    glDeleteBuffers(1, &index);
  }

  GLuint index;
  size_t SIZE;

};

template<typename T>
struct Buffer: public BufferRef {

  typedef T value_type;

  Buffer():BufferRef(){}
  virtual ~Buffer() = default;

  Buffer(const std::vector<T>& buf):BufferRef(){
    fill(buf);
  }

  Buffer(const size_t size, const T* data):BufferRef(){
    fill(size, data);
  }

  template<typename V> void fill(const size_t size, const V* data);
  template<typename V> void fill(const std::vector<V>& buf);
  template<typename V> void fill(const V val);

  void retrieve(size_t size, T* data);
  void retrieve(std::vector<T>& buf);
  void retrieve(T& val);

};

template<typename T>
template<typename V>
void Buffer<T>::fill(const size_t size, const V* data){
  glBindBuffer(GL_ARRAY_BUFFER, index);
  glBufferData(GL_ARRAY_BUFFER, size*sizeof(V), data, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  SIZE = size;
}

template<typename T>
template<typename V>
void Buffer<T>::fill(const std::vector<V>& buf){
  fill(buf.size(), &buf[0]);
}

template<typename T>
template<typename V>
void Buffer<T>::fill(const V val){
  fill(1, &val);
}

template<typename T>
void Buffer<T>::retrieve(size_t size, T* data){
  glBindBuffer(GL_ARRAY_BUFFER, index);
  glGetBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(T), data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Buffer<T>::retrieve(std::vector<T>& buf){
  retrieve(buf.size(), &buf[0]);
}

template<typename T>
void Buffer<T>::retrieve(T& val){
  retrieve(1, &val);
}

} // end of namespace Tiny

#endif
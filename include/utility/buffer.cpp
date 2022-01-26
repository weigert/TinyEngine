struct Buffer {

  GLuint index;
  size_t SIZE;

  Buffer(){
    glGenBuffers(1, &index);
  }

  ~Buffer(){
    glDeleteBuffers(1, &index);
  }

  template<typename T>
  Buffer(std::vector<T> buf):Buffer(){
    fill(buf);
  }

  template<typename T>
  Buffer(std::initializer_list<T> buf):Buffer(){
    fill<T>(buf);
  }

  template<typename T>
  Buffer(size_t size, T* data):Buffer(){
    fill(size, data);
  }

  template<typename T> void fill(size_t size, T* data);
  template<typename T> void fill(std::vector<T> buf);
  template<typename T> void fill(T val);
  template<typename T> void retrieve(size_t size, T* data);
  template<typename T> void retrieve(std::vector<T>& buf);
  template<typename T> void retrieve(T& val);

};

template<typename T>
void Buffer::fill(size_t size, T* data){
  glBindBuffer(GL_ARRAY_BUFFER, index);
  glBufferData(GL_ARRAY_BUFFER, size*sizeof(T), data, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  SIZE = size;
}

template<typename T>
void Buffer::fill(std::vector<T> buf){
  fill(buf.size(), &buf[0]);
}

template<typename T>
void Buffer::fill(T val){
  fill(1, &val);
}

template<typename T>
void Buffer::retrieve(size_t size, T* data){
  glBindBuffer(GL_ARRAY_BUFFER, index);
  glGetBufferSubData(GL_ARRAY_BUFFER, 0, size*sizeof(T), data);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<typename T>
void Buffer::retrieve(std::vector<T>& buf){
  retrieve(buf.size(), &buf[0]);
}

template<typename T>
void Buffer::retrieve(T& val){
  retrieve(1, &val);
}

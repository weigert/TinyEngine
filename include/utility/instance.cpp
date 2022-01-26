class Instance{
private:

  Model* model;                                   //Model Pointer
  std::unordered_map<std::string, int> instances; //Binding Points of Attributes

public:

  size_t SIZE;                                    //Number of Instances
  Instance(Model* m):model(m){};

  template<typename T> void bind(std::string, Buffer*);
  template<typename T> void config(Buffer*);
  void render(GLenum mode = GL_TRIANGLE_STRIP);
  void render(GLenum mode, int size);

};

template<typename T>
void Instance::bind(std::string name, Buffer* buf){
  SIZE = buf->SIZE;
  glBindVertexArray(model->vao);
  glBindBuffer(GL_ARRAY_BUFFER, buf->index);
  config<T>(buf);
  instances[name] = buf->index;
}

template<typename T>
void Instance::config(Buffer* buf){
  glEnableVertexAttribArray(model->bindings.size()+instances.size());
  glVertexAttribPointer(model->bindings.size()+instances.size(), sizeof(T)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribDivisor(model->bindings.size()+instances.size(), 1);
}

template<> //For Matrices - Special Procedure
void Instance::config<glm::mat4>(Buffer* buf){
  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(model->bindings.size()+instances.size()+i);
    glVertexAttribPointer(model->bindings.size()+instances.size()+i, 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(model->bindings.size()+instances.size()+i, 1);
  }
}

void Instance::render(GLenum mode, int size){
  glBindVertexArray(model->vao);
  if(model->indexed){
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->iindex);
    glDrawElementsInstanced(mode, model->SIZE, GL_UNSIGNED_INT, 0, size);
  }
  else glDrawArraysInstanced(mode, 0, model->SIZE, size);
}

void Instance::render(GLenum mode){
  render(mode, SIZE);
}

template<typename T>
class Instance{
public:

  Instance(T* _m){
    m = _m;
  };

  ~Instance(){
    for(auto& b: instances){
        glDeleteBuffers(1, &b);
    }
  }

  T* m;                             //Instanced Render Model

  std::vector<GLuint> instances;    //Instance VBO Pointers
  int SIZE;                         //Number of Instances

  template<typename D>
  void addBuffer(std::vector<D>& buf);
  void addBuffer(std::vector<glm::mat4>& buf);
  template<typename D>
  void updateBuffer(std::vector<D>& buf, int index);
  void updateBuffer(std::vector<glm::mat4>& buf, int index);

  void render(GLenum mode = GL_TRIANGLE_STRIP); //Default because of primitive models
};

template<typename T>
template<typename D>
void Instance<T>::addBuffer(std::vector<D>& buf){

  //Create new VBO
  GLuint instance;
  glGenBuffers(1, &instance);
  SIZE = buf.size();              //Update the Number of Instances

  //Buffer the Data
  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instance);  //Bind Instance Buffer and Data
  glBufferData(GL_ARRAY_BUFFER, SIZE*sizeof(D), &buf[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(m->vbo.size()+instances.size());
  glVertexAttribPointer(m->vbo.size()+instances.size(), sizeof(D)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribDivisor(m->vbo.size()+instances.size(), 1);

  instances.push_back(instance);
}

template<typename T>
template<typename D>
void Instance<T>::updateBuffer(std::vector<D>& buf, int index){
  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instances[index]);  //Bind Instance Buffer and Data
  glBufferSubData(GL_ARRAY_BUFFER, 0, SIZE*sizeof(D), &buf[0]);
}

template<typename T>
void Instance<T>::addBuffer(std::vector<glm::mat4>& buf){

  GLuint instance;
  glGenBuffers(1, &instance);
  SIZE = buf.size();              //Update the Number of Instances

  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instance);  //Bind Instance Buffer and Data
  glBufferData(GL_ARRAY_BUFFER, SIZE*sizeof(glm::mat4), &buf[0], GL_STATIC_DRAW);

  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(m->vbo.size()+instances.size());
    glVertexAttribPointer(m->vbo.size()+instances.size(), 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(m->vbo.size()+instances.size(), 1);
    instances.push_back(instance);
  }
}

template<typename T>
void Instance<T>::updateBuffer(std::vector<glm::mat4>& buf, int index){
  /*
  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instances[index]);  //Bind Instance Buffer and Data
  glBufferSubData(GL_ARRAY_BUFFER, 0, SIZE*sizeof(glm::mat4), &buf[0]);
  */
}

template<typename T>
void Instance<T>::render(GLenum mode){
  glBindVertexArray(m->vao);
  glDrawArraysInstanced(mode, 0, m->SIZE, SIZE); //Instanced render
}

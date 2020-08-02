class Instance{
public:

  Instance(Primitive* _m){
    m = _m;
  };

  ~Instance(){
    for(auto& b: instances){
        glDeleteBuffers(1, &b);
    }
  }

  Primitive* m;                     //Instanced Render Model (must be derived from primitive)
  std::vector<GLuint> instances;    //Instance VBO Pointers
  unsigned int SIZE;                //Number of Instances

  template<typename D> void addBuffer(std::vector<D>& buf);
  template<typename D> void updateBuffer(std::vector<D>& buf, int index);
  template<typename D> void configBuffer(GLuint instance);

  void render(GLenum mode = GL_TRIANGLE_STRIP); //Default because of primitive models
};

template<typename D>
void Instance::addBuffer(std::vector<D>& buf){
  GLuint instance;
  glGenBuffers(1, &instance);
  SIZE = buf.size();              //Update the Number of Instances

  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instance);  //Bind Instance Buffer and Data
  glBufferData(GL_ARRAY_BUFFER, SIZE*sizeof(D), &buf[0], GL_STATIC_DRAW);

  configBuffer<D>(instance);
}

template<typename D>
void Instance::configBuffer(GLuint instance){
  glEnableVertexAttribArray(m->vbo.size()+instances.size());
  glVertexAttribPointer(m->vbo.size()+instances.size(), sizeof(D)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, 0, (void*)0);
  glVertexAttribDivisor(m->vbo.size()+instances.size(), 1);
  instances.push_back(instance);
}

template<> //For Matrices - Special Procedure
void Instance::configBuffer<glm::mat4>(GLuint instance){
  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(m->vbo.size()+instances.size());
    glVertexAttribPointer(m->vbo.size()+instances.size(), 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(m->vbo.size()+instances.size(), 1);
    instances.push_back(instance);
  }
}

template<typename D>
void Instance::updateBuffer(std::vector<D>& buf, int index){
  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instances[index]);  //Bind Instance Buffer and Data
  if(buf.size() != SIZE)  glBufferData(GL_ARRAY_BUFFER, buf.size()*sizeof(D), &buf[0], GL_STATIC_DRAW);
  else                    glBufferSubData(GL_ARRAY_BUFFER, 0, SIZE*sizeof(D), &buf[0]);
  SIZE = buf.size();
}

void Instance::render(GLenum mode){
  glBindVertexArray(m->vao);
  glDrawArraysInstanced(mode, 0, m->SIZE, SIZE); //Instanced render
}

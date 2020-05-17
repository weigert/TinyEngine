template<typename T>
class Particle{
public:

  Particle(T* _m){
    glGenBuffers(1, &instance);
    m = _m;
  };

  ~Particle(){ glDeleteBuffers(1,  &instance); }

  T* m;                             //Instanced Render Model

  GLuint instance;                  //Instance VBO
  std::vector<glm::mat4> models;    //Positions Container
  void update();                    //Bind instance VBO, update model data
  void render(GLenum mode = GL_TRIANGLE_STRIP); //Default because of primitive models
};

template<typename T>
void Particle<T>::update(){
  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instance);  //Bind Instance Buffer and Data
  glBufferData(GL_ARRAY_BUFFER, models.size()*sizeof(glm::mat4), &models[0], GL_STATIC_DRAW);

  for(int i = 0; i < 4; i++){
    glEnableVertexAttribArray(m->vbo.size()+i);
    glVertexAttribPointer(m->vbo.size()+i, 4, GL_FLOAT, GL_FALSE, 4*sizeof(glm::vec4), (void*)(i*sizeof(glm::vec4)));
    glVertexAttribDivisor(m->vbo.size()+i, 1);
  }
}

template<typename T>
void Particle<T>::render(GLenum mode){
  glBindVertexArray(m->vao);
  glDrawArraysInstanced(mode, 0, m->SIZE, models.size()); //Instanced render
}

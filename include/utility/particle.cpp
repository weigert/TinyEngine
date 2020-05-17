template<typename T>
class Particle{
public:

  Particle(T* _m){
    glGenBuffers(1, &instance);
    m = _m;
  };

  ~Particle(){
    glDeleteBuffers(1,  &instance);
  }

  T* m;                             //Instanced Render Model

  GLuint instance;                  //Instance VBO
  std::vector<glm::mat4> models;    //Positions Container
  void update();                    //Bind instance VBO, update model data
  void render(GLenum mode = GL_TRIANGLE_STRIP);
};

template<typename T>
void Particle<T>::update(){
  glBindVertexArray(m->vao);
  glBindBuffer(GL_ARRAY_BUFFER, instance);
  glBufferData(GL_ARRAY_BUFFER, models.size()*sizeof(glm::mat4), &models[0], GL_STATIC_DRAW);

  std::size_t vec4Size = sizeof(glm::vec4);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);
  glVertexAttribDivisor(5, 1);
}

template<typename T>
void Particle<T>::render(GLenum mode){
  glBindVertexArray(m->vao);
  glDrawArraysInstanced(mode, 0, m->SIZE, models.size());
}

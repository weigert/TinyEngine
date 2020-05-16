class Particle{
public:
  Particle(){       //Construct from an SDL Surface
    glGenBuffers(1, &instance);
  };

  ~Particle(){
    glDeleteBuffers(1,  &instance);
  }

  Primitive<SQUARE3D> raw;          //3D Particle System

  GLuint instance;
  std::vector<glm::mat4> models;    //Positions Container
  void update();
  void render();                    //Render to target
};

void Particle::update(){
  glBindVertexArray(raw.vao);
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

void Particle::render(){
  glBindVertexArray(raw.vao);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, models.size());
}

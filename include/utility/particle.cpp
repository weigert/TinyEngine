class Particle{
public:
  Particle(){       //Construct from an SDL Surface
    setup();
  };

  ~Particle(){
    cleanup();
  }

  //Rendering Data
  GLuint vao, vbo[2], instance;

  void setup();
  void update();
  void cleanup();


  //Vertex and Texture Positions
  const GLfloat vert[12] = {-1.0, -1.0,  0.0,
                           -1.0,  1.0,  0.0,
                            1.0, -1.0,  0.0,
                            1.0,  1.0,  0.0};

  const GLfloat tex[8]  = { 0.0,  1.0,
                            0.0,  0.0,
                            1.0,  1.0,
                            1.0,  0.0};

  //Rendering Position
  std::vector<glm::mat4> models;
  void render();                //Render this billboard
};

void Particle::setup(){
  //Setup the VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  //Setup the VBOs
  glGenBuffers(2, &vbo[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  //Setup Instance VBO
  glGenBuffers(1, &instance);
}

void Particle::update(){
  glBindVertexArray(vao);
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

void Particle::cleanup(){
  //Delete Textures and VAO
  glDisableVertexAttribArray(vao);
  glDeleteBuffers(2, vbo);
  glDeleteBuffers(1,  &instance);
  glDeleteVertexArrays(1, &vao);
}

void Particle::render(){
  glBindVertexArray(vao);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, models.size());
}

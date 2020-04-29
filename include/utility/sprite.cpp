class Sprite{
public:
  Sprite(SDL_Surface* s){       //Construct from an SDL Surface
    setup();
    update(s);
  };

  Sprite(){       //Only construct (empty texture)
    setup();
  };

  ~Sprite(){
    cleanup();
  }

  //Rendering Data
  GLuint vao, vbo[2];
  void setup();
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
  glm::vec3 pos = glm::vec3(0.0);
  glm::mat4 model = glm::mat4(1.0f);                  //Model Matrix
  void move(glm::vec2 pos, glm::vec2 scale);

  void update(SDL_Surface* TextureImage);
  void render();                //Render this billboard
};

void Sprite::setup(){
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
}

void Sprite::cleanup(){
  //Delete Textures and VAO
  glDisableVertexAttribArray(vao);
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, &vao);
}

void Sprite::render(){
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

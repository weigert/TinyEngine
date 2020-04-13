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
  GLuint texture;
  void setup();
  void cleanup();

  //Vertex and Texture Positions
  const GLfloat vert[8] = {-1.0, -1.0,
                           -1.0,  1.0,
                            1.0, -1.0,
                            1.0,  1.0};

  const GLfloat tex[8]  = { 0.0,  1.0,
                            0.0,  0.0,
                            1.0,  1.0,
                            1.0,  0.0};

  //Rendering Position
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
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &vert[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), &tex[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  //Generate Textures
  glGenTextures( 1, &texture );
}

void Sprite::cleanup(){
  //Delete Textures and VAO
  glDeleteTextures(1, &texture);
  glDisableVertexAttribArray(vao);
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, &vao);
}

void Sprite::move(glm::vec2 pos, glm::vec2 scale){
  model = glm::translate(glm::mat4(1.0), glm::vec3(2.0*pos.x-1.0+scale.x, 2.0*pos.y-1.0+scale.y, 0.0));
  model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0));
}

/*
================================================================================
                              Texture Constructing
================================================================================
*/

void Sprite::update(SDL_Surface* s){
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

void Sprite::render(){
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

class Billboard{
public:
  Billboard(std::string path){      //Construct from PNG Image
    setup();
    raw(image::load(path));
  };

  Billboard(){
    setup();
  };

  Billboard(int width, int height, bool depthOnly){
    setup();
    drawable(width, height, depthOnly);
  };

  ~Billboard(){
    cleanup();
  }

  //Rendering Data
  unsigned int WIDTH, HEIGHT;
  GLuint vao, vbo[2];
  void setup();
  void cleanup();

  //Vertex and Texture Positions
  const GLfloat vert[8] = {-1.0, -1.0, -1.0,  1.0,  1.0, -1.0,  1.0,  1.0};
  const GLfloat tex[8]  = { 0.0,  0.0,  0.0,  1.0,  1.0,  0.0,  1.0,  1.0};

  //Rendering Position
  glm::mat4 model = glm::mat4(1.0f);                  //Model Matrix
  void move(glm::vec2 pos, glm::vec2 scale);

  //Textures and FBO
  GLuint fbo;       //We need an FBO to render scene to screen
  GLuint texture;
  GLuint depthTexture;

  bool raw(SDL_Surface* TextureImage);
  bool drawable(int width, int height, bool depthOnly);

  void target(glm::vec3 clear); //Billboard as render target with clear color
  void render();                //Render this billboard
};

void Billboard::setup(){
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
  glGenTextures( 1, &depthTexture );
}

void Billboard::cleanup(){
  //Delete all Textures and Framebuffer
  glDeleteTextures(1, &texture);
  glDeleteTextures(1, &depthTexture);
  glDeleteFramebuffers(1, &fbo);

  //Delete the VAO and VBO
  glDisableVertexAttribArray(vao);
  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(1, &vao);
}

void Billboard::move(glm::vec2 pos, glm::vec2 scale){
  model = glm::translate(glm::mat4(1.0), glm::vec3(2.0*pos.x-1.0+scale.x, 2.0*pos.y-1.0+scale.y, 0.0));
  model = glm::scale(model, glm::vec3(scale.x, scale.y, 1.0));
}

/*
================================================================================
                              Texture Constructing
================================================================================
*/

bool Billboard::raw(SDL_Surface* s){
  WIDTH  = s->w;  //Set Width and Height
  HEIGHT = s->h;

  //Update the Texture
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  return true;
}

bool Billboard::drawable(int width, int height, bool depthOnly){
  glGenFramebuffers(1, &fbo); //Frame Buffer Object for drawing
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  if(!depthOnly){
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
  }

  glBindTexture(GL_TEXTURE_2D, depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

  if(depthOnly) glDrawBuffer(GL_NONE);

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
    std::cout<<"Failed to construct framebuffer object."<<std::endl;
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

void Billboard::target(glm::vec3 clear){
  glViewport(0, 0, WIDTH, HEIGHT);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClearColor(clear.x, clear.y, clear.z, 1.0f); //Blue
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Billboard::render(){
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

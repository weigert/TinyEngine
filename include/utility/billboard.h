#pragma once
class Billboard{
public:
  Billboard(std::string path){      //Construct from PNG Image
    setup();
    fromRaw(image::load(path));
  };

  ~Billboard(){
    cleanup();
  }

  //Rendering Data
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
  bool fromRaw(SDL_Surface* TextureImage);
  bool setup(int width, int height);
  bool setup2(int width, int height);

  //Draw
  void render();
};

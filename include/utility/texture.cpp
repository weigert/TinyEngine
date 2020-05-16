class Texture;
using tfunc = std::function<void(Texture*)>;

class Texture{
public:
  Texture(){ glGenTextures( 1, &texture ); };
  Texture(SDL_Surface* s):Texture(){ raw(s); };
  Texture(int W, int H, bool d = false):Texture(){
    if(!d) empty(W, H);
    else depth(W, H);
  };

  ~Texture(){
    glDeleteTextures(1, &texture);
  }

  static tfunc parameter;
  GLuint texture;
  GLenum type = GL_TEXTURE_2D;

  void empty(int WIDTH, int HEIGHT, tfunc param = parameter){
    glBindTexture( type, texture );
    glTexImage2D(type, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    (param)(this);
  }

  void depth(int WIDTH, int HEIGHT, tfunc param = parameter){
    glBindTexture( type, texture );
    glTexImage2D(type, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    (param)(this);
  }

  void raw(SDL_Surface* s, tfunc param = parameter){
    glBindTexture( type, texture );
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(type, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    (param)(this);
  }
};

tfunc Texture::parameter = [](Texture* t){
  glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
};

class Cubetexture: public Texture{
public:
  GLenum type = GL_TEXTURE_CUBE_MAP;

  void empty(int WIDTH, int HEIGHT, tfunc param = parameter){
    glBindTexture(type, texture);
    for(unsigned int i = 0; i < 6; i++)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    (param)(this);
  }

  void depth(int WIDTH, int HEIGHT, tfunc param = parameter){
    glBindTexture(type, texture);
    for(unsigned int i = 0; i < 6; i++)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    (param)(this);
  };
};

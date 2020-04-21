class Texture{
public:
  Texture(SDL_Surface* s){       //Construct from an SDL Surface
    setup();
    raw(s);
  };

  Texture(){       //Only construct (empty texture)
    setup();
  };

  ~Texture(){
    cleanup();
  }

  //Rendering Data
  GLuint texture;
  void setup();
  void cleanup();
  void raw(SDL_Surface* TextureImage);
};

void Texture::setup(){
  glGenTextures( 1, &texture );
}

void Texture::cleanup(){
  glDeleteTextures(1, &texture);
}

void Texture::raw(SDL_Surface* s){
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

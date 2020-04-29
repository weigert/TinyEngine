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
  image::bind(texture, s);
}

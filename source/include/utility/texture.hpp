#ifndef TINYENGINE_UTILITY_TEXTURE
#define TINYENGINE_UTILITY_TEXTURE

class Texture;
using tfunc = std::function<void(Texture*)>; //Arbitrary function operating on texture pointer

struct TextureConfig {
  GLenum internal = GL_RGBA;
  GLenum format = GL_RGBA;
  GLenum type = GL_UNSIGNED_BYTE;
};

class Texture{
public:
  Texture(){ glGenTextures( 1, &texture ); };       //Default constructor
  Texture(int _W, int _H):Texture(){ W = _W; H = _H; };   //Construct w. Size
  Texture(SDL_Surface* s):Texture(s->w, s->h){ raw(s); }; //Load raw surface data into texture
  Texture(int _W, int _H, TextureConfig TC, void* data = NULL):Texture(_W, _H){
    setup(TC, data);
  }

  ~Texture(){ glDeleteTextures(1, &texture); }

  GLuint texture;               //Texture int (OpenGL: everything is an int!)
  GLenum type = GL_TEXTURE_2D;  //Texture type (default is this)
  int W, H;                     //Texture Width, Height
  static tfunc parameter;       //Texture Parameter Setter!

  void setup(TextureConfig TC, void* data = NULL);
  void raw(SDL_Surface* s);

};

void Texture::setup(TextureConfig TC, void* data){
  glBindTexture( type, texture );
  glTexImage2D( type, 0, TC.internal, W, H, 0, TC.format, TC.type, data );
  Texture::parameter(this);
}

void Texture::raw(SDL_Surface* s){  //Generate a texture from raw surface data
  setup({GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE}, s->pixels);
  SDL_FreeSurface(s);
}

//Default parameter setting function!
//Note that you can pass your own to the texture building functions above!z
tfunc Texture::parameter = [](Texture* t){
  glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//  glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//  glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
};

class Cubetexture: public Texture {
public:
  Cubetexture():Texture(){ type = GL_TEXTURE_CUBE_MAP; };
  Cubetexture(int _W, int _H):Cubetexture(){ W = _W; H = _H; };
  Cubetexture(int _W, int _H, TextureConfig TC, void* data = NULL):Cubetexture(_W, _H){
    setup(TC, data);
  }
  void setup(TextureConfig TC, void* data = NULL);
};

void Cubetexture::setup(TextureConfig TC, void* data){
  glBindTexture( type, texture );
  for(unsigned int i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, TC.internal, W, H, 0, TC.format, TC.type, data);
  Texture::parameter(this);
}

#endif

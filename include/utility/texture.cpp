#ifndef TINYENGINE_TEXTURE
#define TINYENGINE_TEXTURE

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

class Cubetexture: public Texture{  //Cubetexture specialization.
public:                             //Same thing, 6 times
  Cubetexture():Texture(){ type = GL_TEXTURE_CUBE_MAP; };

  Cubetexture(int W, int H, bool d = false):Cubetexture(){  //Create empty texture of defined size
    if(!d) empty(W, H);
    else   depth(W, H);
  };
  Cubetexture(int _W, int _H):Cubetexture(){ W = _W; H = _H; };
  Cubetexture(int _W, int _H, TextureConfig TC, void* data = NULL):Cubetexture(_W, _H){
    setup(TC, data);
  }
  void setup(TextureConfig TC, void* data = NULL);
};

void Texture::empty(int W, int H, tfunc param, GLenum F){
  glBindTexture( type, texture );
  glTexImage2D(type, 0, F, W, H, 0, F, GL_UNSIGNED_BYTE, NULL);
  (param)(this);              //Call Parameter Setter
}

void Texture::depth(int W, int H, tfunc param){
  empty(W, H, param, GL_DEPTH_COMPONENT);
}

void Texture::raw(SDL_Surface* s, tfunc param){  //Generate a texture from raw surface data

  glBindTexture( type, texture );

  /*
  //   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glActiveTexture(GL_TEXTURE0 + boundtextures);


          {
      GLenum err = glGetError();
      if(err != GL_NO_ERROR){
        std::cout<<err<<std::endl;
      }
      else std::cout<<"AYY4"<<std::endl;
    }
    */

  glTexImage2D(type, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);



  (param)(this); //Call the parameter setting function!
  SDL_FreeSurface(s);



}

//Default parameter setting function!
//Note that you can pass your own to the texture building functions above!z
tfunc Texture::parameter = [](Texture* t){
  glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
};

void Cubetexture::empty(int W, int H, tfunc param, GLenum F){
  glBindTexture(type, texture);
  for(unsigned int i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, F, W, H, 0, F, GL_UNSIGNED_BYTE, NULL);
  (param)(this);
}

void Cubetexture::depth(int W, int H, tfunc param){
  empty(W, H, param, GL_DEPTH_COMPONENT);
};

#endif

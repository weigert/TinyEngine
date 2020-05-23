class Texture;
using tfunc = std::function<void(Texture*)>; //Arbitrary function operating on texture pointer

class Texture{
public:
  Texture(){ glGenTextures( 1, &texture ); };       //Default constructor
  Texture(SDL_Surface* s):Texture(){ raw(s); };     //Load raw surface data into texture
  Texture(int W, int H, bool d = false):Texture(){  //Create empty texture of defined size
    if(!d) empty(W, H);
    else   depth(W, H);
  };

  ~Texture(){ glDeleteTextures(1, &texture); }

  static tfunc parameter;       //Texture Parameter Setter!
  GLuint texture;               //Texture int (OpenGL: everything is an int!)
  GLenum type = GL_TEXTURE_2D;  //Texture type (default is this)

  void empty(int W, int H, tfunc param = parameter, GLenum F = GL_RGBA){
    glBindTexture( type, texture );
    glTexImage2D(type, 0, F, W, H, 0, F, GL_UNSIGNED_BYTE, NULL);
    (param)(this);              //Call Parameter Setter
  }

  void depth(int W, int H, tfunc param = parameter){
    empty(W, H, param, GL_DEPTH_COMPONENT);
  }

  void raw(SDL_Surface* s, tfunc param = parameter){  //Generate a texture from raw surface data
    glBindTexture( type, texture );
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(type, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
    (param)(this); //Call the parameter setting function!
  }
};

//Default parameter setting function!
//Note that you can pass your own to the texture building functions above!z
tfunc Texture::parameter = [](Texture* t){
  glTexParameteri(t->type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
};

class Cubetexture: public Texture{  //Cubetexture specialization.
public:                             //Same thing, 6 times
  Cubetexture():Texture(){
    type = GL_TEXTURE_CUBE_MAP;
  };

  Cubetexture(int W, int H, bool d = false):Cubetexture(){  //Create empty texture of defined size
    if(!d) empty(W, H);
    else   depth(W, H);
  };

  void empty(int W, int H, tfunc param = parameter, GLenum F = GL_RGBA){
    glBindTexture(type, texture);
    for(unsigned int i = 0; i < 6; i++)
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, F, W, H, 0, F, GL_UNSIGNED_BYTE, NULL);
    (param)(this);
  }

  void depth(int W, int H, tfunc param = parameter){
    empty(W, H, param, GL_DEPTH_COMPONENT);
  };
};

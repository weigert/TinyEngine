class Texture;
using tfunc = std::function<void(Texture*)>; //Arbitrary function operating on texture pointer

class Texture{
public:
  Texture(){ glGenTextures( 1, &texture ); };       //Default constructor
  Texture(SDL_Surface* s):Texture(){ raw(s); };     //Load raw surface data into texture
  Texture(int W, int H, bool d = false):Texture(){  //Create empty texture of defined size
    if(!d) empty(W, H);
    else depth(W, H);
  };

  ~Texture(){ glDeleteTextures(1, &texture); }

  static tfunc parameter;       //Texture Parameter Setter!
  GLuint texture;               //Texture int (OpenGL: everything is an int!)
  GLenum type = GL_TEXTURE_2D;  //Texture type (default is this)

  void empty(int WIDTH, int HEIGHT, tfunc param = parameter){ //Generate an empty texture (color bit)
    glBindTexture( type, texture );
    glTexImage2D(type, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    (param)(this); //Call the parameter setting function!
  }

  void depth(int WIDTH, int HEIGHT, tfunc param = parameter){ //Generate an empty texture (depth bit)
    glBindTexture( type, texture );
    glTexImage2D(type, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    (param)(this); //Call the parameter setting function!
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
  glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
};

class Cubetexture: public Texture{ //Cubetexture specialization. Does the same things, just 6 times
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

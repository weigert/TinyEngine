class Target {
public:

  Target(){ glGenFramebuffers(1, &fbo); };              //Default constructor

  Target(int W, int H, bool d = false, bool c = true)   //Construct with a size
  :Target(){
    WIDTH = W; HEIGHT = H;
    dAttach = d;
    cAttach = c;
  }

  ~Target(){ glDeleteFramebuffers(1, &fbo); }           //Default destructor

  unsigned int WIDTH, HEIGHT;
  bool dAttach = false, cAttach = true;                 //Whether we have a depth / color attachment
  GLuint fbo;                                           //FBO (OpenGL: everything is an int)

  template<typename T>
  void bind(T& t, bool d = false){                      //Bind a texture to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if(d) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, t.texture, 0);
    else  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  template<typename T>
  void setup(T& t, T& d){                               //Add color and depth textures!

    if(dAttach){
      d.depth(WIDTH, HEIGHT); //Setup the textures as empty!!!
      bind(d, true);          //Then bind
    }
    if(cAttach){
      t.empty(WIDTH, HEIGHT);
      bind(t, false);
    }
    else{                     //If we don't have a color attachment, don't draw to it
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout<<"Framebuffer Incomplete"<<std::endl;

  }

  template<typename T>
  void target(T a);       //With a clear-color
  void target(){          //This FBO as a render target (no clearing)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, WIDTH, HEIGHT);
  }
};

template<typename T>
void Target::target(T a){
  target();
  glClearColor(a[0], a[1], a[2], 1.0f);
  if(dAttach) glClear( GL_DEPTH_BUFFER_BIT );
  if(cAttach) glClear( GL_COLOR_BUFFER_BIT );
}

class Billboard: public Target{   //Billboard specialization
public:
  Texture texture, depth;         //Two normal textures

  Billboard(int W, int H, bool d = false, bool c = true){
    setup(texture, depth);        //Bind the two normal textures to the billboard
  }

  Billboard(SDL_Surface* s){      //Render target preloaded with an image
    WIDTH = s->w; HEIGHT = s->h;
    texture.raw(s);               //Construct the texture from raw surface data
    bind(texture, false);         //Bind it to the billboard as color texture
  }
};

class Cubemap: public Target{     //Cubemap specialization
public:
  Cubetexture texture, depth;     //Two cubemap textures

  Cubemap(int W, int H, bool d = false, bool c = true){
    setup(texture, depth);        //Bind the two cubemap textures to the target
  }
};

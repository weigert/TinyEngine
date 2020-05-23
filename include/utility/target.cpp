class Target {
public:
  Target(){ glGenFramebuffers(1, &fbo); };              //Default constructor

  Target(int W, int H, bool c = false, bool d = true):  //Construct with a size
    Target(){ WIDTH = W; HEIGHT = H; dAttach = d; cAttach = c; }

  ~Target(){ glDeleteFramebuffers(1, &fbo); }           //Default destructor

  unsigned int WIDTH, HEIGHT;
  bool dAttach = false, cAttach = true;                 //Whether we have a depth / color attachment
  GLuint fbo;                                           //FBO (OpenGL: everything is an int)

  template<typename T>
  void bind(T& t, bool d){                              //Bind a texture to the FBO
    if(d) glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, t.texture, 0);
    else  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, t.texture, 0);
  }

  template<typename T>
  void setup(T& t, T& d){                               //Add color and depth textures!
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if(dAttach) bind(d, true);
    if(cAttach) bind(t, false);
    else{ //If we don't have a color attachment, don't draw to it
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout<<"Framebuffer Incomplete"<<std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void target(bool t = true){          //This FBO as a render target (no clearing)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, WIDTH, HEIGHT);
    if(t && dAttach) glClear( GL_DEPTH_BUFFER_BIT );
    if(t && cAttach) glClear( GL_COLOR_BUFFER_BIT );
  }
  template<typename T> void target(T a){
    glClearColor(a[0], a[1], a[2], 1.0f);
    target();
  }
};

class Billboard: public Target{   //Billboard specialization
public:
  Texture texture, depth;         //Two normal textures

  Billboard(int W, int H, bool c = true, bool d = true):
  Target(W, H, c, d){
    if(dAttach) depth.depth(WIDTH, HEIGHT);
    if(cAttach) texture.empty(WIDTH, HEIGHT);
    setup(texture, depth);        //Bind the two normal textures to the billboard
  }

  Billboard(SDL_Surface* s):      //Render target preloaded with an image
  Billboard(s->w, s->h, true, false){
    texture.raw(s);               //Construct the texture from raw surface data
    bind(texture, false);         //Bind it to the billboard as color texture
  }
};

class Cubemap: public Target{     //Cubemap specialization
public:
  Cubetexture texture, depth;     //Two cubemap textures

  Cubemap(int W, int H, bool c = true, bool d = true):Target(W, H, c, d){
    if(dAttach) depth.depth(WIDTH, HEIGHT);
    if(cAttach) texture.empty(WIDTH, HEIGHT);
    setup(texture, depth);
  }
};

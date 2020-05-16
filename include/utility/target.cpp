class Target {
public:

  Target(){ glGenFramebuffers(1, &fbo); };

  Target(int W, int H, bool d = false, bool c = true){
    Target();
    WIDTH = W; HEIGHT = H;
    dAttach = d;
    cAttach = c;
  }

  ~Target(){
    glDeleteFramebuffers(1, &fbo);
  }

  unsigned int WIDTH, HEIGHT;
  bool dAttach = false, cAttach = true;
  GLuint fbo;

  template<typename T>
  void bind(T& t, bool d){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if(d) glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, t.texture, 0);
    else  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t.texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  template<typename T>
  void setup(T& t, T& d){

    if(dAttach){
      d.depth(WIDTH, HEIGHT);
      bind(d, true);
    }
    if(cAttach){
      t.empty(WIDTH, HEIGHT);
      bind(t, false);
    }
    else{
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout<<"Framebuffer Incomplete"<<std::endl;

  }

  void target();
  template<typename T>
  void target(T a);
};

void Target::target(){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, WIDTH, HEIGHT);
}

template<typename T>
void Target::target(T a){
  target();
  glClearColor(a[0], a[1], a[2], 1.0f);
  if(dAttach) glClear( GL_DEPTH_BUFFER_BIT );
  if(cAttach) glClear( GL_COLOR_BUFFER_BIT );
}

class Billboard: public Target{
public:
  Texture texture, depth;

  Billboard(int W, int H, bool d = false, bool c = true){
    setup(texture, depth);
  }

  Billboard(SDL_Surface* s){
    WIDTH = s->w; HEIGHT = s->h;
    texture.raw(s);
    bind(texture, false);
  }
};

class Cubemap: public Target{
public:
  Cubetexture texture, depth;

  Cubemap(int W, int H, bool d = false, bool c = true){
    setup(texture, depth);
  }
};

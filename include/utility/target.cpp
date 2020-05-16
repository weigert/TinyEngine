template<GLenum T>
class Target{
public:

  Target(){
    glGenFramebuffers(1, &fbo);
  };

  Target(int W, int H, bool _d){
    glGenFramebuffers(1, &fbo);
    setup(W, H, _d);
  };

  ~Target(){
    glDeleteFramebuffers(1, &fbo);
  }

  unsigned int WIDTH, HEIGHT;
  bool depthOnly;
  GLuint fbo;
  Texture texture, depth;

  void setup(int W, int H, bool _d);

  void target();
  void target(glm::vec3 clear);
  void target(float* clear);
};

template<GLenum T>
void Target<T>::setup(int W, int H, bool _d){
  WIDTH = W; HEIGHT = H;
  depthOnly = _d;

  if(!depthOnly){
    texture.empty<T>(WIDTH, HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.texture, 0);
  }

  depth.depth<T>(WIDTH, HEIGHT);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth.texture, 0);

  if(depthOnly){
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template<GLenum T>
void Target<T>::target(){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClear(GL_DEPTH_BUFFER_BIT);
}

template<GLenum T>
void Target<T>::target(glm::vec3 clear){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(clear.x, clear.y, clear.z, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

template<GLenum T>
void Target<T>::target(float* a){
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, WIDTH, HEIGHT);
  glClearColor(a[0], a[1], a[2], 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

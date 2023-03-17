#ifndef TINYENGINE_UTILITY_TARGET
#define TINYENGINE_UTILITY_TARGET
#include <TinyEngine/Texture>

class Target {
public:
  Target(int W, int H){
    WIDTH = W; HEIGHT = H;
    glGenFramebuffers(1, &fbo);
  }

  ~Target(){ glDeleteFramebuffers(1, &fbo); }           //Default destructor

  unsigned int WIDTH, HEIGHT;
  GLuint fbo;                                           //FBO (OpenGL: everything is an int)

  std::vector<GLenum> attached;
  bool hasdepth = false;

  template<typename T>
  void bind(T& t, GLenum attach = GL_COLOR_ATTACHMENT0){  //Bind a texture to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, attach, t.texture, 0);
    if(attach != GL_DEPTH_ATTACHMENT){
      attached.push_back(attach);
      glDrawBuffers(attached.size(), &attached[0]);
    } else {
      hasdepth = true;
      if(attached.empty())      // Optimization: Attach depth buffer, but no color
        glDrawBuffer(GL_NONE);  // Don't Output Fragment Shader Stuff
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  bool check(){
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      return false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
  }

  void target(bool t = true){          //This FBO as a render target (no clearing)
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, WIDTH, HEIGHT);
    if(t) glClear( GL_DEPTH_BUFFER_BIT );
    if(t) glClear( GL_COLOR_BUFFER_BIT );
  }

  template<typename T> void target(T a){
    glClearColor(a[0], a[1], a[2], 1.0f);
    target();
  }

  template<typename T>                 //Raw Buffer Sampling
  void sample(T* m, glm::vec2 p, glm::vec2 d, GLenum ATTACH = GL_COLOR_BUFFER_BIT, GLenum FORMAT = GL_RGBA){
    if(d.x <= 0 || d.y <= 0 || p.x+d.x > WIDTH || p.y+d.y > HEIGHT) return;
    glBindFramebuffer(GL_FRAMEBUFFER, fbo); glReadBuffer(ATTACH);
    glReadPixels(p.x, p.y, d.x, d.y, FORMAT, GL_UNSIGNED_BYTE, m);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
};

class Billboard: public Target{   //Billboard specialization
public:
  Texture texture, depth;         //Two normal textures

  Billboard(int _W, int _H):
  texture(_W, _H), depth(_W, _H), Target(_W, _H){
    texture.setup({GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE});
    bind(texture, GL_COLOR_ATTACHMENT0);
    depth.setup({GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE});
    bind(depth, GL_DEPTH_ATTACHMENT);
  }
  ~Billboard(){ glDeleteFramebuffers(1, &fbo); }             //Default destructor

  Billboard(SDL_Surface* s):      //Render target preloaded with an image
  Billboard(s->w, s->h){
    texture.raw(s);               //Construct the texture from raw surface data
    bind(texture, GL_COLOR_ATTACHMENT0);         //Bind it to the billboard as color texture
  }

};

class Cubemap: public Target{     //Cubemap specialization
public:
  Cubetexture texture, depth;     //Two cubemap textures

  Cubemap(int _W, int _H):
  texture(_W, _H), depth(_W, _H), Target(_W, _H) {
    texture.setup({GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE});
    bind(texture, GL_COLOR_ATTACHMENT0);
    depth.setup({GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE});
    bind(depth, GL_DEPTH_ATTACHMENT);
  }
};

#endif

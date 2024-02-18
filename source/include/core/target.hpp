#ifndef TINYENGINE_CORE_TARGET
#define TINYENGINE_CORE_TARGET

#include <TinyEngine/TinyEngine>
#include <TinyEngine/Texture>

namespace Tiny {

//! \brief Target is a Frame-Buffer wrapper that can be targeted 
//! for off-screen rendering. 
//!
//! A target can bind multiple textures as depth or color
//! attachments, which can be individually rendered to
//! during the fragment shader stage.
//!
//! This allows for multi-stage rendering pipelines.
//!
struct Target {
protected:
  Target(){ glGenFramebuffers(1, &_index); }      //!< Allocated GPU Frame Buffer Object

public:
  ~Target(){ glDeleteFramebuffers(1, &_index); }  //! De-Allocate GPU Frame Buffer Object

  //! Construct a Target w. Width and Height
  Target(const size_t width, const size_t height):Target(){
    this->_width = width;
    this->_height = height;
  }

  //! Attach a Texture to the Target
  void bind(Texture& texture, const GLenum attachment = GL_COLOR_ATTACHMENT0);
  void target();                                              //!< Set this Target as the Current Render Target
  void clear(const glm::vec4 color = glm::vec4{0, 0, 0, 1});  //!< Clear this Target (Optional Clear Color)
  bool valid();                                               //!< Validate FBO Setup Status

  void operator()() const { glBindFramebuffer(GL_FRAMEBUFFER, this->index()); } //!< Bind the Framebuffer

  inline const uint32_t index() const { return this->_index; }  //!< Retrieve Target Frame Buffer Index
  inline const size_t width()   const { return this->_width; }  //!< Retrieve Target Width
  inline const size_t height()  const { return this->_height; } //!< Retrieve Target Height

private:
  uint32_t _index;                  //!< Local Frame Buffer Object Index
  size_t _width;                    //!< Local Target Width
  size_t _height;                   //!< Local Target Height
  std::vector<GLenum> attachments;  //!< Local Attachment Map
};

void Target::bind(Texture& texture, const GLenum attachment){  
  this->operator()();
  glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture.index(), 0);
  if(attachment != GL_DEPTH_ATTACHMENT){
    this->attachments.push_back(attachment);
    glDrawBuffers(attachments.size(), &attachments[0]);
  }
  else if(attachments.empty())
    glDrawBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Target::clear(const glm::vec4 color){
  this->operator()();
  glClearColor(color[0], color[1], color[2], color[3]);
  glClear(GL_DEPTH_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Target::target(){
  this->operator()();
  glViewport(0, 0, width(), height());
}

bool Target::valid(){
  this->operator()();
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    return false;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

// Implementations

//! \brief Billboard is a 2D Target with an RGBA-Color and Depth Texture
struct Billboard: Target {

  Billboard(const size_t width, const size_t height):Target(width, height),
  _color(width, height, Texture::RGBA8U),
  _depth(width, height, Texture::DEPTH8U){
    this->bind(_color, GL_COLOR_ATTACHMENT0);
    this->bind(_depth, GL_DEPTH_ATTACHMENT);
  }

  inline Texture& color() { return this->_color; }
  inline Texture& depth() { return this->_depth; }

private:
  Texture _color; //!< Billboard Color Attachment (RGBA8U)
  Texture _depth; //!< Billboard Depth Attachment (DEPTH8U)
};

//! \brief CubeMap is a 3D Target with an RGBA-Color and Depth Texture
struct CubeMap: Target {

  CubeMap(const size_t width, const size_t height):Target(width, height),
  _color(width, height, Texture::RGBA8U),
  _depth(width, height, Texture::DEPTH8U){
    this->bind(_color, GL_COLOR_ATTACHMENT0);
    this->bind(_depth, GL_DEPTH_ATTACHMENT);
  }

  inline CubeTexture& color() { return this->_color; }  //!< Retrieve Color CubeTexture Attachment (Texture::RGBA8U)
  inline CubeTexture& depth() { return this->_depth; }  //!< Retrieve Depth CubeTexture Attachment (Texture::DEPTH8U)

private:
  CubeTexture _color;  //!< CubeMap Color Attachment (RGBA8U)
  CubeTexture _depth;  //!< CubeMap Depth Attachment (DEPTH8U)
};

//! \brief GBuffer is a Target with multiple Texture color attachments
//!
//! This struct is a correctly constructed Framebuffer for deffered rendering.
struct GBuffer: Target {

  //! Construct GBuffer with width, height
  GBuffer(const size_t width, const size_t height):
  Target(width, height),
  _position(width, height, Texture::RGBA16F),
  _normal(width, height, Texture::RGBA16F),
  _color(width, height, Texture::RGBA8U),
  _depth(width, height, Texture::DEPTH8U){
    this->bind(_position, GL_COLOR_ATTACHMENT0);
    this->bind(_normal, GL_COLOR_ATTACHMENT1);
    this->bind(_color, GL_COLOR_ATTACHMENT2);
    this->bind(_depth, GL_DEPTH_ATTACHMENT);
  }

  inline Texture& position() { return this->_position; }  //!< Retrieve Position Texture Attachment (Texture::RGBA16F)
  inline Texture& normal() { return this->_normal; }      //!< Retrieve Normal Texture Attachment (Texture::RGBA16F)
  inline Texture& color() { return this->_color; }        //!< Retrieve Color Texture Attachment (Texture::RGBA8U)
  inline Texture& depth() { return this->_depth; }        //!< Retrieve Depth Texture Attachment (Texture::DEPTH8U)

private:
  Texture _position;  //!< GBuffer Position Attachment (RGBA16F)
  Texture _normal;    //!< GBuffer Normal Attachment (RGBA16F)
  Texture _color;     //!< GBuffer Color Attachment (RGBA8U)
  Texture _depth;     //!< GBuffer Depth Attachment (DEPTH8U)
};

} // end of namespace Tiny

#endif
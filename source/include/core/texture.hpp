#ifndef TINYENGINE_CORE_TEXTURE
#define TINYENGINE_CORE_TEXTURE

#include <TinyEngine/TinyEngine>

namespace Tiny {

//! Texture is GPU Texture of a fixed size and data Layout.
//!
//! A Texture makes multi-dimensional data available to shaders
//! for sampling with built-in interpolation and boundary handling.
//!
//! Note that textures are read-only. In order to write to textures,
//! they can be bound to a Target, which can be rendered onto using shaders.
//!
struct Texture {

  //! Layout specifies the Data Format of GPU Textures.
  //!
  struct Layout {
    GLenum internal;  //!< Defines number of Components
    GLenum format;    //!< Defines Pixel Components
    GLenum type;      //!< Defines Pixel Component Format
  };

protected:
  Texture(){ glGenTextures(1, &_index); };    //!< Allocate GPU Texture
  
public:
  ~Texture(){ glDeleteTextures(1, &_index); } //!< De-Allocate GPU Texture

  //! Allocate GPU Texture with Size
  Texture(const size_t width, const size_t height):
  Texture(){
    this->_width = width;
    this->_height = height;
  }

  //! Construct Texture width Size, Layout and Data
  Texture(const size_t width, const size_t height, Layout layout, const void* data = NULL):
  Texture(width, height){
    this->set(layout, data);
  }

  void set(Layout layout, const void* data = NULL);                               //!< Fill Texture with Layout of Data
  void operator()() const { glBindTexture(GL_TEXTURE_2D, this->index()); }        //!< Bind the Texture

  inline const uint32_t index()  const { return this->_index; }                   //!< Retrieve Texture Index
  inline const size_t width()    const { return this->_width; }                   //!< Retrieve Texture Width
  inline const size_t height()   const { return this->_height; }                  //!< Retrieve Texture Height
  inline const glm::ivec2 size() const { return glm::ivec2(width(), height()); }  //!< Retrieve Texture Size
  
  // Static Layout Specifications

  static constexpr Layout R8U{GL_R8, GL_RED, GL_UNSIGNED_BYTE}; 
  static constexpr Layout RGBA8U{GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE}; 
  static constexpr Layout RGBA16F{GL_RGBA16F, GL_RGBA, GL_FLOAT};
  static constexpr Layout RGBA32F{GL_RGBA32F, GL_RGBA, GL_FLOAT};
  static constexpr Layout DEPTH8U{GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE};

protected:
  uint32_t _index;  //!< Local Texture Index
  size_t _width;    //!< Texture Width (Pixels)
  size_t _height;   //!< Texture Height (Pixels)
};

void Texture::set(Layout layout, const void* data){
  this->operator()();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, layout.internal, width(), height(), 0, layout.format, layout.type, data);
}

// CubeTexture Specialization

//! CubeTexture is a combination of 6 Textures, which can be
//! easily sampled in a shader by a direction vector.
//!
//! This allows for e.g. sampling of directional information.
//!
struct CubeTexture: Texture {
protected:
  CubeTexture():Texture(){};

public:

  CubeTexture(const size_t width, const size_t height):Texture(width, height){}
  CubeTexture(const size_t width, const size_t height, Layout layout, void* data = NULL):
  CubeTexture(width, height){
    this->set(layout, data);
  }

  void set(Layout layout, const void* data = NULL);                               //!< Fill CubeTexture with Layout of Data
  void operator()() const { glBindTexture(GL_TEXTURE_CUBE_MAP, this->index()); }  //!< Bind the CubeTexture
};

void CubeTexture::set(Layout layout, const void* data){
  this->operator()();
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  for(unsigned int i = 0; i < 6; i++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, layout.internal, width(), height(), 0, layout.format, layout.type, data);
}

} // end of namespace Tiny

#endif

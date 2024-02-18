#ifndef TINYENGINE_UTIL_CAMERA
#define TINYENGINE_UTIL_CAMERA

#include <TinyEngine/TinyEngine>

namespace Tiny {

namespace cam {

  struct perspective; //!< Perspective Projection
  struct orthogonal;  //!< Orthogonal Projection
  
  struct orbit; //!< Orbit Control
  struct free;  //!< Free-Cam Controls

};

//! Camera Type, Composed of Controls and Projection
template<typename P, typename C>
struct camera {

  typedef P project_t;
  typedef C control_t;

  camera(const project_t& project, const control_t& control):
  project(project),
  control(control){
    this->update();
  }

  void update(){
    this->control.update();
    this->project.update();
    this->_vp = project.proj()*control.view();
  }

  void hook(){
    this->project.hook();
    this->control.hook();
    Tiny::event.finalize([this](){
      this->update();
    });
  }

  inline glm::mat4 vp() const { return this->_vp; }

  control_t control;
  project_t project;
protected:
  glm::mat4 _vp;
};

// Camera Projection Implementations

//! Orthogonal Projection
struct cam::orthogonal {

  orthogonal(glm::vec2 dim, glm::vec2 clip, float scale):
  dim(dim),
  clip(clip),
  scale(scale){
    this->update();
  }

  void update(){
    this->_proj = glm::ortho(-dim.x/scale, dim.x/scale, -dim.y/scale, dim.y/scale, clip.x, clip.y);
  }

  void hook(){

    Tiny::event.active[SDLK_PAGEUP]([this](){
      scale /= 0.98;
    });

    Tiny::event.active[SDLK_PAGEDOWN]([this](){
      this->scale *= 0.98;
    });

    Tiny::event.scroll([this](glm::ivec2 dir){
      if(dir.y >=  1) this->scale /= 0.98;
      if(dir.y <= -1) this->scale *= 0.98;
    });

    Tiny::event.resize([this](glm::ivec2 dim){
      this->dim = dim;
    });

  }

  inline glm::mat4 proj() const { return this->_proj; }

protected:
  glm::vec2 dim;
  glm::vec2 clip;
  float scale;
  glm::mat4 _proj;
};

// Camera Control Implementations

//! Camera Orbit Controls
struct cam::orbit {

  static constexpr float tau = 2.0*3.14159265;

  orbit(glm::vec3 _pos, glm::vec3 _look = glm::vec3(0)):
  _pos(_pos),
  _look(_look){
    const glm::vec3 dir = _pos - _look;
    r = glm::length(dir);
    phi = std::atan2(dir.z, dir.x);
    theta = std::acos(dir.y/r);
    this->update();
  }

  void update(){
    this->_pos = _look + this->r*glm::vec3(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));
    this->_view = glm::lookAt(_pos, _look, glm::vec3(0,1,0));
    this->_invview = glm::inverse(_view);
  }

  inline const glm::vec3 look() const {
    return this->_look;
  }

  inline const float _phi() const {
    return this->phi;
  }

  inline const glm::vec3 pos() const {
    return this->_pos;
  }

  inline const glm::mat4 view() const {
    return this->_view;
  }

  inline const glm::mat4 invview() const {
    return this->_invview;
  }

  // Implementations

  void pan(const float& inc){
    phi += inc;
    if(phi > tau) phi -= tau;
    if(phi < 0) phi += tau;
  }

  void zoom(const float& inc){
  //  r += inc;
  }

  void tilt(const float& inc){
    theta += inc;
    if(theta >= 0.5*tau) theta =  0.5*tau - 0.00001;
    if(theta <= 0.0f) theta = 0.00001f;
  }

  void stride(const float& inc){
    _look.x += inc*sin(phi);
    _look.z -= inc*cos(phi);
  }

  void strafe(const float& inc){
    _look.x += inc*cos(phi);
    _look.z += inc*sin(phi);
  }

  void rise(const float& inc){
    _look.y += inc;
  }

  const float zoomrate = 0.1;
  const float moverate = 0.05;
  const float turnrate = 0.1;

  void hook(){

    Tiny::event.scroll([this](glm::ivec2 dir){
      if(dir.x >=  1) this->pan( turnrate);
      if(dir.x <= -1) this->pan(-turnrate);
    });

    Tiny::event.active[SDLK_UP]([this](){
      this->tilt(-0.1*turnrate);
    });

    Tiny::event.active[SDLK_DOWN]([this](){
      this->tilt(+0.1*turnrate);
    });

    Tiny::event.active[SDLK_LEFT]([this](){
      this->pan(0.1*turnrate);
    });

    Tiny::event.active[SDLK_RIGHT]([this](){
      this->pan(-0.1*turnrate);
    });

    Tiny::event.active[SDLK_c]([this](){
      this->rise(-moverate);
    });

    Tiny::event.active[SDLK_v]([this](){
      this->rise(moverate);
    });

    Tiny::event.active[SDLK_w]([this](){
      this->stride(-moverate);
    });

    Tiny::event.active[SDLK_s]([this](){
      this->stride(moverate);
    });

    Tiny::event.active[SDLK_d]([this](){
      this->strafe(-moverate);
    });

    Tiny::event.active[SDLK_a]([this](){
      this->strafe(moverate);
    });

    Tiny::event.finalize([this](){
      this->update();
    });

  }

private:
  float r;
  float phi;
  float theta;
  glm::vec3 _pos;
  glm::vec3 _look;
  glm::mat4 _view;
  glm::mat4 _invview;
};

} // end of namespace Tiny

#endif

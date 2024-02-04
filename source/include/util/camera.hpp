#ifndef TINYENGINE_UTIL_CAMERA
#define TINYENGINE_UTIL_CAMERA

namespace Tiny {
namespace cam {

//! Camera Type, Composed of Controls and Projection
template<typename P, typename C>
struct camera {

  typedef P projection_type;
  typedef C control_type;

  camera(const projection_type& projection, const control_type& control):
    projection(projection),control(control){
      this->update();
    }

  void update(){
    this->control.update();
    this->projection.update();
    this->_vp = projection.proj()*control.view();
  }

  const std::function<void()> handler = [&](){
    control.handler();
    projection.handler();
    this->update();
  };

  const inline glm::mat4 vp(){
    return this->_vp;
  }

  control_type control;
  projection_type projection;
private:
  glm::mat4 _vp;
};

// Camera Projection Implementations

struct ortho {

  ortho(const float& W, const float& H, const float& near, const float& far, const float& scale):
    W(W),H(H),near(near),far(far),rad(scale){
    this->update();
  }

  void update(){
    this->_proj = glm::ortho(-W/rad, W/rad, -H/rad, H/rad, near, far);
  }

  inline const glm::mat4 proj() const {
    return this->_proj;
  }

  void handler(){

    if(Tiny::event.scroll.posy) this->rad += 0.1;
    if(Tiny::event.scroll.negy) this->rad -= 0.1;
    
    if(Tiny::event.active[SDLK_PAGEUP])   this->rad += 0.1;
    if(Tiny::event.active[SDLK_PAGEDOWN]) this->rad -= 0.1;

    if(Tiny::event.windowEventTrigger){
      this->W = Tiny::view.WIDTH;
      this->H = Tiny::view.HEIGHT;
    }

  }

private:
  float W, H, rad, near, far;
  glm::mat4 _proj;
};

// Camera Control Implementations

struct orbit {

  static constexpr float tau = 2.0*3.14159265;

  orbit(glm::vec3 _pos, glm::vec3 _look = glm::vec3(0)):
    _pos(_pos),_look(_look){
      this->update();
      // r = ;
      // phi = ;
      // theta = ;
    }

  void update(){
    this->_pos = _look + this->r*glm::vec3(cos(theta)*sin(phi), sin(theta), cos(theta)*cos(phi));
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
    r += inc;
  }

  void tilt(const float& inc){
    theta += inc;
    if(theta >=  0.25*tau) theta =  0.25*tau - inc;
    if(theta <= -0.25*tau) theta = -0.25*tau - inc;
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

  void handler(){

    if(Tiny::event.scroll.posy)
      this->zoom( zoomrate);

    if(Tiny::event.scroll.negy)
      this->zoom(-zoomrate);

    if(Tiny::event.scroll.posx)
      this->pan( turnrate);

    if(Tiny::event.scroll.negx)
      this->pan(-turnrate);

    if(Tiny::event.active[SDLK_UP])
      this->tilt(0.1*turnrate);

    if(Tiny::event.active[SDLK_DOWN])
      this->tilt(-0.1*turnrate);

    if(Tiny::event.active[SDLK_LEFT])
      this->pan(-0.1*turnrate);

    if(Tiny::event.active[SDLK_RIGHT])
      this->pan(0.1*turnrate);

    if(Tiny::event.active[SDLK_c])
      this->rise(-moverate);

    if(Tiny::event.active[SDLK_v])
      this->rise(moverate);

    if(Tiny::event.active[SDLK_w])
      this->stride(-moverate);

    if(Tiny::event.active[SDLK_s])
      this->stride(moverate);

    if(Tiny::event.active[SDLK_d])
      this->strafe(-moverate);

    if(Tiny::event.active[SDLK_a])
      this->strafe(moverate);

    if(Tiny::event.active[SDLK_PAGEUP])
      this->zoom( zoomrate);

    if(Tiny::event.active[SDLK_PAGEDOWN])
      this->zoom(-zoomrate);
  }

private:
  float r = 1.0f;
  float phi = 0.0f;
  float theta = 0.0f;
  glm::vec3 _pos;
  glm::vec3 _look;
  glm::mat4 _view;
  glm::mat4 _invview;
};

// Unimplemented
struct perspective{};
struct free{};

} // end of namespace cam
} // end of namespace Tiny

#endif

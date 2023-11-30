#ifndef TINYENGINE_UTIL_CAMERA
#define TINYENGINE_UTIL_CAMERA

namespace Tiny {

//! Camera Control Type Enumerator
enum camera_control {
  FREE,
  ORBIT
};

//! Camera Projetion Type Enumerator
enum camera_projection {
  ORTHOGONAL,
  PERSPECTIVE
};

template<camera_control C>
struct cam_control;

template<camera_projection P>
struct cam_projection;

//! Camera Type, Composed of Controls and Projection
template<camera_projection P, camera_control C>
  struct camera {

  typedef cam_projection<P> projection_type;
  typedef cam_control<C> control_type;

  camera(const projection_type& projection, const control_type& control):
    projection(projection),control(control){
      this->update();
    }

  const std::function<void()> handler = [&](){
    control.handler();
  };

  const inline glm::mat4 vp(){
    return this->_vp;
  }

  void update(){
    this->control.update();
    this->_vp = projection.proj()*control.view();
  }

  control_type control;
private:
  projection_type projection;
  glm::mat4 _vp;
};

template<>
struct cam_projection<camera_projection::ORTHOGONAL> {

  //cam_projection(float rad){
  //  this->_proj = glm::ortho(-(float)Tiny::view.WIDTH/rad, (float)Tiny::view.WIDTH/rad, -(float)Tiny::view.HEIGHT/rad, (float)Tiny::view.HEIGHT/rad, near, far);
  //}

  cam_projection(const glm::mat4& _proj):
    _proj(_proj){}

  inline const glm::mat4 proj() const {
    return this->_proj;
  }

private:
  glm::mat4 _proj;
};



template<>
struct cam_control<camera_control::ORBIT> {

  static constexpr float tau = 2.0*3.14159265;

  cam_control(glm::vec3 _pos, glm::vec3 _look = glm::vec3(0)):
    _pos(_pos),_look(_look){
      // this->update();
      // r = ;
      // phi = ;
      // theta = ;
    }

  void update(){
    this->_pos = _look + this->r*glm::vec3(cos(theta)*sin(phi), sin(theta), cos(theta)*cos(phi));
    this->_view = glm::lookAt(_pos, _look, glm::vec3(0,1,0));
    this->_invview = glm::inverse(_view);
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
    this->update();
  }

  /*
  void zoom(const float& inc){
    r += inc;
    this->update();
  }

  void tilt(const float& inc){
    theta += inc;
    if(theta >=  0.25*tau) theta =  0.25*tau - inc;
    if(theta <= -0.25*tau) theta = -0.25*tau - inc;
    this->update();
  }

  void stride(const float& inc){
    _look.x += inc*sin(phi);
    _look.z -= inc*cos(phi);
    this->update();
  }

  void strafe(const float& inc){
    _look.x += inc*cos(phi);
    _look.z += inc*sin(phi);
    this->update();
  }

  void rise(const float& inc){
    _look.y += inc;
    this->update();
  }

  const float zoomrate = 0.1;
  const float moverate = 0.1;
  const float turnrate = 0.1;
  */

  void handler(){

    /*

    if(Tiny::event.scroll.posy)
      this->zoom( zoomrate);

    if(Tiny::event.scroll.negy)
      this->zoom(-zoomrate);

    if(Tiny::event.scroll.posx)
      this->pan( turnrate);

    if(Tiny::event.scroll.negx)
      this->pan(-turnrate);

    if(Tiny::event.active[SDLK_UP])
      this->tilt(turnrate);

    if(Tiny::event.active[SDLK_DOWN])
      this->tilt(-turnrate);

    if(Tiny::event.active[SDLK_LEFT])
      this->pan(turnrate);

    if(Tiny::event.active[SDLK_RIGHT])
      this->pan(-turnrate);

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
      */
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

}

#endif

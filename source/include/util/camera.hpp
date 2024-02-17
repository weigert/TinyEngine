#ifndef TINYENGINE_UTIL_CAMERA
#define TINYENGINE_UTIL_CAMERA

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

  const std::function<void()> handler = [this](){
    this->control.handler();
    this->project.handler();
    this->update();
  };

  inline glm::mat4 vp() const { return this->_vp; }

protected:
  control_t control;
  project_t project;
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

  void handler(){

    if(Tiny::event.scroll.posy) this->scale += 0.1;
    if(Tiny::event.scroll.negy) this->scale -= 0.1;
    
    if(Tiny::event.active[SDLK_PAGEUP])   this->scale += 0.1;
    if(Tiny::event.active[SDLK_PAGEDOWN]) this->scale -= 0.1;

    if(Tiny::event.windowEventTrigger){
      this->dim.x = Tiny::view.WIDTH;
      this->dim.y = Tiny::view.HEIGHT;
    }

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
    r = glm::length(_look - _pos);
    // phi = ;
    // theta = ;
    this->update();
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

} // end of namespace Tiny

#endif

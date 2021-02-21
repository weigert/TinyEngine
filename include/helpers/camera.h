namespace cam {
enum camtype {ORTHO, PROJ};

using namespace glm;

  float rad = 20.0f;     //Camera Distance to Viewpoint
  float rot = 0.0f;     //Camera Rotation around Y-Axis
  float roty = 0.0f;    //Camera Rotation around X/Z-Axis

  float near = 0.01f;    //Clipping Planes
  float far = 10.0f;

  vec3 pos = vec3(cos(glm::radians(rot)), sin(glm::radians(roty)), sin(glm::radians(rot)));
  vec3 look = vec3(0);

  camtype type = PROJ;

  //Matrices
  mat4 proj, view, vp;

  void update(){
    pos = vec3(cos(glm::radians(roty))*cos(glm::radians(rot)), sin(glm::radians(roty)), cos(glm::radians(roty))*sin(glm::radians(rot)));
    view = lookAt(look+rad*pos, look, vec3(0,1,0));
    vp = proj*view;
  }

  void init(float r = 5.0f, camtype t = PROJ){
    type = t;
    rad = r;

    if(type == PROJ) proj = glm::perspective(45.0f, (float)Tiny::view.WIDTH/(float)Tiny::view.HEIGHT, near, far);
    if(type == ORTHO) proj = glm::ortho(-Tiny::view.WIDTH/100.0f, Tiny::view.WIDTH/100.0f, -Tiny::view.HEIGHT/100.0f, Tiny::view.HEIGHT/100.0f, near, far);
    view = lookAt(look+rad*pos, look, vec3(0,1,0));
    vp = proj*view;
  }

  /* Rotations */

  void zoom(float inc){
    rad += inc;
    update();
  }

  void pan(float inc){
    rot = (rot+inc+360.0f)-(int)((rot+inc+360.0f)/360.0f)*360.0f;
    update();
  }

  void tilt(float inc){
    roty = (roty+inc+360.0f)-(int)((roty+inc+360.0f)/360.0f)*360.0f;
    update();
  }

  /* Translations */

  void stride(float inc){
    look.x += inc*cos(glm::radians(rot));
    look.z += inc*sin(glm::radians(rot));
    update();
  }

  void strafe(float inc){
    look.x += inc*sin(glm::radians(rot));
    look.z += inc*cos(glm::radians(rot));
    update();
  }

  void rise(float inc){
    look.y += inc;
    update();
  }


  std::function<void()> handler = [](){
    if(Tiny::event.scroll.posy)
      cam::zoom(0.5);

    if(Tiny::event.scroll.negy)
      cam::zoom(-0.5);

    if(Tiny::event.scroll.posx)
      cam::pan(1.5f);

    if(Tiny::event.scroll.negx)
      cam::pan(-1.5f);

    if(Tiny::event.active[SDLK_UP])
      cam::tilt(1.5f);

    if(Tiny::event.active[SDLK_DOWN])
      cam::tilt(-1.5f);

    if(Tiny::event.active[SDLK_c])
      cam::rise(-0.1f);

    if(Tiny::event.active[SDLK_v])
      cam::rise(0.1f);

    if(Tiny::event.active[SDLK_w])
      cam::stride(-0.1f);

    if(Tiny::event.active[SDLK_s])
      cam::stride(0.1f);

  };

}

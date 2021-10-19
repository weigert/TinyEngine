namespace cam {
enum camtype {ORTHO, PROJ};

using namespace glm;

  float rad = 20.0f;     //Camera Distance to Viewpoint
  float rot = 0.0f;     //Camera Rotation around Y-Axis
  float roty = 0.0f;    //Camera Rotation around X/Z-Axis

  float near = 0.01f;    //Clipping Planes
  float far = 10.0f;
  float FOV = 1.0f;       //Field of View

  vec3 pos = vec3(cos(glm::radians(roty))*sin(glm::radians(rot)), sin(glm::radians(roty)), -cos(glm::radians(roty))*cos(glm::radians(rot)));

  vec3 look = vec3(0);

  float zoomrate = 0.5f;
  float moverate = 1.0f;
  float turnrate = 1.5f;

  bool moved = true;

  camtype type = PROJ;

  //Matrices
  mat4 proj, view, vp, invview;

  //Pathing Matrix Setting
  void move(mat4 v){ view = v; vp = proj*view; }
  void shift(mat4 rt){ view = rt*view; vp = proj*view; }

  void update(){
//    vec3 pos = vec3(sin(glm::radians(rot)), sin(glm::radians(roty)), cos(glm::radians(rot)));

    pos = vec3(cos(glm::radians(roty))*sin(glm::radians(rot)), sin(glm::radians(roty)), -cos(glm::radians(roty))*cos(glm::radians(rot)));

    if(type == PROJ) view = lookAt(look+rad*pos, look, vec3(0,1,0));
    if(type == ORTHO) view = lookAt(look+pos, look, vec3(0,1,0));
    invview = inverse(view);

    vp = proj*view;
  }

  void init(float r = 5.0f, camtype t = PROJ){
    type = t;
    rad = r;

    if(type == PROJ) proj = glm::perspective(FOV, (float)Tiny::view.WIDTH/(float)Tiny::view.HEIGHT, near, far);
    if(type == ORTHO) proj = glm::ortho(-(float)Tiny::view.WIDTH/rad, (float)Tiny::view.WIDTH/rad, -(float)Tiny::view.HEIGHT/rad, (float)Tiny::view.HEIGHT/rad, near, far);

    if(type == PROJ) view = lookAt(look+rad*pos, look, vec3(0,1,0));
    if(type == ORTHO) view = lookAt(look+pos, look, vec3(0,1,0));
    invview = inverse(view);

    vp = proj*view;
  }

  /* Rotations */

  void zoom(float inc){
    rad += inc;
    if(type == ORTHO) proj = glm::ortho(-(float)Tiny::view.WIDTH/rad, (float)Tiny::view.WIDTH/rad, -(float)Tiny::view.HEIGHT/rad, (float)Tiny::view.HEIGHT/rad, near, far);
    update();
    moved = true;
  }

  void pan(float inc){
    rot = (rot+inc+360.0f)-(int)((rot+inc+360.0f)/360.0f)*360.0f;
    update();
    moved = true;
  }

  void tilt(float inc){
    roty += inc;
    if(roty >= 90.0f) roty = 90.0f-inc;
    if(roty <= -90.0f) roty = -90.0f-inc;
    update();
    moved = true;
  }

  /* Translations */

  void stride(float inc){
    look.x += inc*sin(glm::radians(rot));
    look.z -= inc*cos(glm::radians(rot));
    update();
    moved = true;
  }

  void strafe(float inc){
    look.x += inc*cos(glm::radians(rot));
    look.z += inc*sin(glm::radians(rot));
    update();
    moved = true;
  }

  void rise(float inc){
    look.y += inc;
    update();
    moved = true;
  }


  std::function<bool()> handler = [](){

    moved = false;

    if(Tiny::event.scroll.posy)
      cam::zoom(cam::zoomrate);

    if(Tiny::event.scroll.negy)
      cam::zoom(-cam::zoomrate);

    if(Tiny::event.scroll.posx)
      cam::pan(cam::turnrate);

    if(Tiny::event.scroll.negx)
      cam::pan(-cam::turnrate);

    if(Tiny::event.active[SDLK_UP])
      cam::tilt(cam::turnrate);

    if(Tiny::event.active[SDLK_DOWN])
      cam::tilt(-cam::turnrate);

    if(Tiny::event.active[SDLK_LEFT])
      cam::pan(cam::turnrate);

    if(Tiny::event.active[SDLK_RIGHT])
      cam::pan(-cam::turnrate);

    if(Tiny::event.active[SDLK_c])
      cam::rise(-cam::moverate);

    if(Tiny::event.active[SDLK_v])
      cam::rise(cam::moverate);

    if(Tiny::event.active[SDLK_w])
      cam::stride(-cam::moverate);

    if(Tiny::event.active[SDLK_s])
      cam::stride(cam::moverate);

    if(Tiny::event.active[SDLK_d])
      cam::strafe(-cam::moverate);

    if(Tiny::event.active[SDLK_a])
      cam::strafe(cam::moverate);

    if(Tiny::event.windowEventTrigger){
      if(type == PROJ) cam::proj = glm::perspective(FOV, (float)Tiny::view.WIDTH/(float)Tiny::view.HEIGHT, near, far);
      if(type == ORTHO) proj = glm::ortho(-(float)Tiny::view.WIDTH/rad, (float)Tiny::view.WIDTH/rad, -(float)Tiny::view.HEIGHT/rad, (float)Tiny::view.HEIGHT/rad, near, far);
      cam::vp = cam::view*cam::proj;
      moved = true;
      update();
    }

    return moved;

  };

}

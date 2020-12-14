namespace cam {
using namespace glm;

  float scale = 0.05f;
  float rot = 0.0f;

  vec3 pos = vec3(50);
  vec3 look = vec3(0);

  mat4 proj, view, vp;

  void init(vec3 p = vec3(50), vec3 l = vec3(0), float s = 0.05f){
    pos = p; look = l; scale = s;
    proj = ortho(-(float)Tiny::view.WIDTH*scale, (float)Tiny::view.WIDTH*scale, -(float)Tiny::view.HEIGHT*scale, (float)Tiny::view.HEIGHT*scale, -800.0f, 500.0f);
    view = lookAt(pos, look, vec3(0,1,0));
    vp = proj*view;
  }

  void zoom(float inc){
    scale += inc;
    proj = ortho(-(float)Tiny::view.WIDTH*scale, (float)Tiny::view.WIDTH*scale, -(float)Tiny::view.HEIGHT*scale, (float)Tiny::view.HEIGHT*scale, -800.0f, 500.0f);
    vp = proj*view;
  }

  void pan(float inc){
    rot += inc;
    rot = (rot+360.0f)-(int)((rot+360.0f)/360.0f)*360.0f;
    view = rotate(view, radians(inc), vec3(0, 1, 0));
    vp = proj*view;
  }

}

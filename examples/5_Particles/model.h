const int WIDTH = 800;
const int HEIGHT = 800;

float zoom = 0.05;
float zoomInc = 0.001;
float rotation = 0.0f;
glm::vec3 cameraPos = glm::vec3(50, 50, 50);
glm::vec3 lookPos = glm::vec3(0, 0, 0);
glm::mat4 camera = glm::lookAt(cameraPos, lookPos, glm::vec3(0,1,0));
glm::mat4 projection;

void setup(){
  projection = glm::ortho(-(float)Tiny::view.WIDTH*zoom, (float)Tiny::view.WIDTH*zoom, -(float)Tiny::view.HEIGHT*zoom, (float)Tiny::view.HEIGHT*zoom, -800.0f, 500.0f);
};

std::function<void()> eventHandler = [&](){

  if(Tiny::event.scroll.posy && zoom <= 0.3){
    zoom += zoomInc;
    projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -800.0f, 500.0f);
  }
  if(Tiny::event.scroll.negy && zoom > 0.005){
    zoom -= zoomInc;
    projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -800.0f, 500.0f);
  }
  if(Tiny::event.scroll.posx){
    rotation += 1.5f;
    if(rotation < 0.0) rotation = 360.0 + rotation;
    else if(rotation > 360.0) rotation = rotation - 360.0;
    camera = glm::rotate(camera, glm::radians(1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
  }
  if(Tiny::event.scroll.negx){
    rotation -= 1.5f;
    if(rotation < 0.0) rotation = 360.0 + rotation;
    else if(rotation > 360.0) rotation = rotation - 360.0;
    camera = glm::rotate(camera, glm::radians(-1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
  }

};

//Data
const int SIZE = 500;
int data[SIZE*SIZE] = {0};
bool paused = true;
float PI = 3.14159265;

//Resolution
int res = 25;
float radius = 2.0;
float bias[2] = {0.0};
double zoom = 1.5;
int order = 2;

float cc[4] = {0.070, 0.086, 0.211, 1.0};
float dc[4] = {0.882, 0.157, 0.204, 1.0};

bool active = false;
bool appear = false;
float appearAmp = 1.0;
bool movex = false;
bool movey = false;
float phase[2] = {0.0, 0.0};
float amplitude[2] = {1.0, 1.0};
float center[2] = {0.0, 0.0};
float rate = 0.01;
float t = 0.0;
glm::vec3 up = glm::vec3(0,1,0);

glm::vec3 camerapos = glm::vec3(5.0);
glm::vec3 lookpos = glm::vec3(0.0);

// Event Handler
std::function<void()> eventHandler = [&](){

  if(Tiny::event.scroll.posy)
    zoom /= 0.9;
  if(Tiny::event.scroll.negy)
    zoom *= 0.9;
  if(Tiny::event.scroll.posx)
    camerapos = glm::rotate(glm::mat4(1),  0.05f, up)*glm::vec4(camerapos, 1.0);
  if(Tiny::event.scroll.negx)
    camerapos = glm::rotate(glm::mat4(1), -0.05f, up)*glm::vec4(camerapos, 1.0);

};

//Interface Function
Handle interfaceFunc = [&](){
  //Window Size
  ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50, 470), ImGuiCond_Once);

  //Open Window
  ImGui::Begin("Raymarching Controller", NULL, ImGuiWindowFlags_NoResize);

    ImGui::ColorEdit3("Intersect Color", cc);
    ImGui::ColorEdit3("Miss Color", dc);

  ImGui::End();
};

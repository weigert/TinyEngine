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
double px = 0.0;
double py = 0.0;

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

// Event Handler
std::function<void()> eventHandler = [&](){

  if(Tiny::event.scroll.posy)
    zoom *= 0.9;
  if(Tiny::event.scroll.negy)
    zoom /= 0.9;

  if(Tiny::event.active[SDLK_w])
    py += 0.05*zoom;

  if(Tiny::event.active[SDLK_a])
    px += 0.05*zoom;

  if(Tiny::event.active[SDLK_s])
    py -= 0.05*zoom;

  if(Tiny::event.active[SDLK_d])
    px -= 0.05*zoom;

};

//Interface Function
Handle interfaceFunc = [&](){
  //Window Size
  ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50, 470), ImGuiCond_Once);

  //Open Window
  ImGui::Begin("Julia Set Controller", NULL, ImGuiWindowFlags_NoResize);

    if(ImGui::BeginTabBar("Tab Bar", ImGuiTabBarFlags_None)){
      //Current Position Information
      if(ImGui::BeginTabItem("Info")){
        ImGui::Text("Position:"); ImGui::SameLine();
        ImGui::Text("( %f,", px); ImGui::SameLine();
        ImGui::Text(" %f )", py);
        ImGui::Text("Zoom: "); ImGui::SameLine();
        ImGui::Text("%f", zoom);
        ImGui::EndTabItem();
      }
      if(ImGui::BeginTabItem("Visual")){
        ImGui::ColorEdit3("Converge Color", cc);
        ImGui::ColorEdit3("Diverge Color", dc);
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

  ImGui::End();
};

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
    py -= 0.05*zoom;

  if(Tiny::event.active[SDLK_a])
    px += 0.05*zoom;

  if(Tiny::event.active[SDLK_s])
    py += 0.05*zoom;

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
        if(ImGui::Button("Randomize")){ //Randomize
          t = 0.0;  //Reset Time
          radius = 1.0;
          active = true;
          appearAmp = 1.0+(float)(rand()%200)/1000.0;
          rate = 0.002;
          order = rand()%4+2; //2-6
          appear = rand()%2;
          movex = rand()%2;
          movey = rand()%2;
          res = rand()%15+10;
          center[0] = (float)(rand()%2000)/1000.0-1.0;
          center[1] = (float)(rand()%2000)/1000.0-1.0;
          phase[0] = (float)(rand()%2000)/1000.0-1.0;
          phase[1] = (float)(rand()%2000)/1000.0-1.0;
          amplitude[0] = (float)(rand()%800)/1000.0+0.2;
          amplitude[1] = (float)(rand()%800)/1000.0+0.2;
        }
        ImGui::EndTabItem();
      }
      if(ImGui::BeginTabItem("Control")){
        ImGui::DragInt("Order", &order, 1, 0, 10);
        ImGui::DragInt("Resolution", &res, 1, 0, 500);
        ImGui::DragFloat("Convergence", &radius, 0.01f, 0.0f, 2.0f);
        ImGui::DragFloat2("Bias", bias, 0.005f, -10.0f, 10.0f);
        ImGui::EndTabItem();
      }
      if(ImGui::BeginTabItem("Visual")){
        ImGui::ColorEdit3("Converge Color", cc);
        ImGui::ColorEdit3("Diverge Color", dc);
        ImGui::EndTabItem();
      }
      if(ImGui::BeginTabItem("Animate")){
        ImGui::DragFloat("Rate", &rate, 0.0001f, 0.001f, 0.01f);
        ImGui::SameLine(); ImGui::Checkbox("Run", &active);
        ImGui::Checkbox("Appear", &appear); ImGui::SameLine();
        ImGui::Checkbox("Move X", &movex); ImGui::SameLine();
        ImGui::Checkbox("Move Y", &movey);
        ImGui::DragFloat2("Phase", phase, 0.001f, -1.0f, 1.0f);
        ImGui::DragFloat2("Amplitude", amplitude, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat2("Center", center, 0.001f, -10.0f, 10.0f);
        ImGui::DragFloat("Appear Amplitude", &appearAmp, 0.01f, 0.0f, 2.0f); ImGui::SameLine();

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

  ImGui::End();
};

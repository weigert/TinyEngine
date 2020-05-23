//Resolution
double zoom = 1.5;
double px = 0.0;
double py = 0.0;

float cc[4] = {0.070, 0.086, 0.211, 1.0};
float dc[4] = {0.882, 0.157, 0.204, 1.0};

float density = 0.1;
float thickness = 0.05;
bool drawlines = true;

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
  ImGui::Begin("2D Signed Distance Field Controller", NULL, ImGuiWindowFlags_NoResize);
    ImGui::ColorEdit3("Negative Color", dc);
    ImGui::ColorEdit3("Positive Color", cc);
    ImGui::Text("Position:"); ImGui::SameLine();
    ImGui::Text("( %f,", px); ImGui::SameLine();
    ImGui::Text(" %f )", py);
    ImGui::Text("Zoom: "); ImGui::SameLine();
    ImGui::Text("%f", zoom);
    ImGui::Checkbox("Draw Isolines", &drawlines);
    ImGui::SliderFloat("Thickness", &thickness, 0.01f, 1.0f);
    ImGui::SliderFloat("Density", &density, 0.02f, 1.0f);

  ImGui::End();
};

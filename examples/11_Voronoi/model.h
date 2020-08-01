//GPU Accelerated Voronoi Controller Stuff

bool drawcenter = false;

bool animate = true;
bool updated = false;
bool translate = false;

//Choose how to Filter!
bool drawdepth = false;
bool drawcolor = true;
bool drawpick = false;
int drawstyle;

std::vector<glm::vec2> centroids;
std::vector<glm::vec2> offset;

float K = 256;
float R = 2.0f*sqrt(4.0f/3.14159265f/K);

std::function<void()> interfaceFunc = [&](){


  ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50, 470), ImGuiCond_Once);

  ImGui::Begin("Controller", NULL, ImGuiWindowFlags_NoResize);

  ImGui::Checkbox("Animate", &animate); ImGui::SameLine();
  ImGui::Checkbox("Centroids", &drawcenter);

  static int e;
  ImGui::RadioButton("Pick", &e, 0); ImGui::SameLine();
  ImGui::RadioButton("Depth", &e, 1); ImGui::SameLine();
  ImGui::RadioButton("Mosaic", &e, 2); ImGui::SameLine();
  ImGui::RadioButton("Bubble", &e, 3);

  ImGui::Checkbox("Non-Static", &translate);

  drawstyle = e;

  ImGui::SliderFloat("Centroids", &K, 256, 16*1024);
  if(ImGui::Button("Re-Seed")){
    centroids.clear();
    offset.clear();
    sample::disc(centroids, K, glm::vec2(-1), glm::vec2(1));
    offset = centroids;
    R = 2.0f*sqrt(4.0f/3.14159265f/K);
    updated = true;
  }

  ImGui::End();

};

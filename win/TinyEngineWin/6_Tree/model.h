//Parameters
#define PI 3.14159265f

const int WIDTH = 1200;
const int HEIGHT = 800;

float zoom = 0.5;
float zoomInc = 0.99;
float rotation = 0.0f;
glm::vec3 cameraPos = glm::vec3(50, 200, 50);
glm::vec3 lookPos = glm::vec3(0, 180, 0);
glm::mat4 camera = glm::lookAt(cameraPos, lookPos, glm::vec3(0,1,0));
glm::mat4 projection;

bool paused = false;
bool autorotate = true;
bool drawwire = false;
bool drawtree = true;
bool drawleaf = true;

float leafcolor[3] = {0.82, 0.13, 0.23};
float treecolor[3] = {1.00, 1.00, 1.00};
float wirecolor[3] = {0.00, 0.00, 0.00};
float backcolor[3] = {0.80, 0.80, 0.80};
float lightcolor[3] = {1.00, 1.00, 1.00};
float leafopacity = 0.9;
int leafmindepth = 8;
float treeopacity = 1.0;
float treescale[2] = {15.0f, 5.0f};

int ringsize = 12;
int leafcount = 10;
float leafsize = 5.0;
float taper = 0.6;
float leafspread[3] = {50.0, 50.0, 50.0};

float growthrate = 1.0;
float passratio = 0.3;
float splitdecay = 1E-2;
float directedness = 0.5;
int localdepth = 2;
bool conservearea = true;

bool drawshadow = true;
bool selfshadow = true;
bool leafshadow = true;
glm::vec3 lightpos = glm::vec3(50);
glm::mat4 bias = glm::mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
);
glm::mat4 lproj = glm::ortho(-300.0f, 300.0f, -300.0f, 400.0f, -200.0f, 800.0f);
glm::mat4 lview = glm::lookAt(lightpos, glm::vec3(0), glm::vec3(0,1,0));

#include "tree.h"

void setup(){
  projection = glm::ortho(-(float)Tiny::view.WIDTH*zoom, (float)Tiny::view.WIDTH*zoom, -(float)Tiny::view.HEIGHT*zoom, (float)Tiny::view.HEIGHT*zoom, -500.0f, 800.0f);
  srand(time(NULL));
  root = new Branch({0.6, 0.45, 2.5}); //Create Root
}

// Event Handler
std::function<void()> eventHandler = [&](){

  if(Tiny::event.scroll.posy){
    zoom /= zoomInc;
    projection = glm::ortho(-(float)Tiny::view.WIDTH*zoom, (float)Tiny::view.WIDTH*zoom, -(float)Tiny::view.HEIGHT*zoom, (float)Tiny::view.HEIGHT*zoom, -500.0f, 800.0f);
  }
  if(Tiny::event.scroll.negy){
    zoom *= zoomInc;
    projection = glm::ortho(-(float)Tiny::view.WIDTH*zoom, (float)Tiny::view.WIDTH*zoom, -(float)Tiny::view.HEIGHT*zoom, (float)Tiny::view.HEIGHT*zoom, -500.0f, 800.0f);
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

  //Pause Toggle
  if(!Tiny::event.press.empty()){
    if(Tiny::event.press.back() == SDLK_p)
      paused = !paused;
    else if(Tiny::event.press.back() == SDLK_a)
      autorotate = !autorotate;

    //Regrow
    else if(Tiny::event.press.back() == SDLK_r){
      Branch* newroot = new Branch(root, true);
      delete(root);
      root = newroot;
    }
  }

};

//Interface Function
Handle interfaceFunc = [&](){
  //Window Size
  ImGui::SetNextWindowSize(ImVec2(360, 400), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Once);

  //Open Window
  ImGui::Begin("Tree Controller", NULL, ImGuiWindowFlags_None);
    if(ImGui::BeginTabBar("Tab Bar", ImGuiTabBarFlags_None)){
      if(ImGui::BeginTabItem("Info")){

        ImGui::Checkbox("Pause [P]", &paused);
        ImGui::Checkbox("Auto-Rotate [A]", &autorotate);

        ImGui::ColorEdit3("Background", backcolor);
//        ImGui::ColorEdit3("Light Color", lightcolor);

        ImGui::Text("Made by Nicholas McDonald");
        ImGui::EndTabItem();

      }

      if(ImGui::BeginTabItem("Growth")){

        if(ImGui::Button("Re-Grow [R]")){
          Branch* newroot = new Branch(root, true);
          delete(root);
          root = newroot;
        }

        ImGui::Text("Growth Behavior");
        ImGui::DragFloat("Growth Rate", &growthrate, 0.01f, 0.0f, 5.0f);
        ImGui::Checkbox("Conserve Crossectional Area", &conservearea);
        if(!conservearea)
          ImGui::DragFloat("Pass Ratio", &passratio, 0.01f, 0.0f, 1.0f);

        ImGui::Text("Split Behavior");
        ImGui::DragFloat("Ratio", &root->ratio, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Size", &root->splitsize, 0.1f, 0.1f, 5.0f);
        ImGui::DragFloat("Decay", &splitdecay, 0.001f, 0.0f, 1.0f);

        ImGui::Text("Growth Direction");
        ImGui::DragFloat("Spread", &root->spread, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Directedness", &directedness, 0.01f, 0.0f, 1.0f);
        ImGui::DragInt("Local Depth", &localdepth, 1, 0, 15);

        ImGui::EndTabItem();
      }

      if(ImGui::BeginTabItem("Leaf")){
        ImGui::ColorEdit3("Color", leafcolor);
        ImGui::DragFloat("Opacity", &leafopacity, 0.01f, 0.0f, 1.0f);
        ImGui::DragInt("Count", &leafcount, 1, 0, 30);
        ImGui::DragInt("Minimum Depth", &leafmindepth, 1, 0, 15);
        ImGui::DragFloat3("Spread", leafspread, 0.1f, 0.0f, 250.0f);
        ImGui::DragFloat("Size", &leafsize, 0.1f, 0.0f, 25.0f);
        ImGui::Checkbox("Draw", &drawleaf); ImGui::SameLine();
        ImGui::Checkbox("Shade", &leafshadow); ImGui::SameLine();
        ImGui::Checkbox("Self-Shade", &selfshadow);

        ImGui::EndTabItem();
      }

      if(ImGui::BeginTabItem("Tree")){
        ImGui::ColorEdit3("Fill", treecolor);
        ImGui::ColorEdit3("Wire", wirecolor);
        ImGui::DragFloat("Opacity", &treeopacity, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat2("Scale", treescale, 0.01f, 0.1f, 50.0f);
        ImGui::DragFloat("Taper", &taper, 0.01f, 0.0f, 1.0f);

        ImGui::Checkbox("Draw", &drawtree); ImGui::SameLine();
        ImGui::Checkbox("Wire", &drawwire); ImGui::SameLine();
        ImGui::Checkbox("Shade", &drawshadow);

        ImGui::DragInt("Mesh", &ringsize, 1, 3, 12);

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

  ImGui::End();
};

std::function<void(Model* m)> construct_floor = [&](Model* h){

  float floor[24] = {
    -1.0, 0.0, -1.0,
    -1.0, 0.0,  1.0,
     1.0, 0.0, -1.0,
     1.0, 0.0,  1.0,
  };

  for(int i = 0; i < 12; i++)
    h->positions.push_back(floor[i]);

  h->indices.push_back(0);
  h->indices.push_back(1);
  h->indices.push_back(2);

  h->indices.push_back(1);
  h->indices.push_back(3);
  h->indices.push_back(2);

  glm::vec3 floorcolor = glm::vec3(0.65, 0.5, 0.3);

  for(int i = 0; i < 4; i++){
    h->normals.push_back(0.0);
    h->normals.push_back(1.0);
    h->normals.push_back(0.0);

    h->colors.push_back(floorcolor.x);
    h->colors.push_back(floorcolor.y);
    h->colors.push_back(floorcolor.z);
    h->colors.push_back(1.0);
  }
};

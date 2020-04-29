//Parameters
const int WIDTH = 800;
const int HEIGHT = 800;

float zoom = 0.5;
float zoomInc = 0.99;
float rotation = 0.0f;
glm::vec3 cameraPos = glm::vec3(50, 200, 50);
glm::vec3 lookPos = glm::vec3(0, 200, 0);
glm::mat4 camera = glm::lookAt(cameraPos, lookPos, glm::vec3(0,1,0));
glm::mat4 projection;

bool paused = false;
bool autorotate = false;
bool drawwire = false;
bool drawtree = true;
bool drawleaf = true;

float leafcolor[3] = {0.34, 0.545, 0.46};
float treecolor[3] = {1.00, 1.00, 1.00};
float wirecolor[3] = {0.00, 0.00, 0.00};
float backcolor[3] = {0.80, 0.80, 0.80};
float leafopacity = 0.5;
float treeopacity = 1.0;
float treescale[2] = {15.0f, 1.0f};

int ringsize = 6;
int leafcount = 10;
float leafsize = 5.0;
float taper = 0.6;
float leafspread[3] = {150.0, 50.0, 150.0};

float passratio = 0.8;
bool conservearea = false;
bool conservediameter = false;

#include "tree.h"

void setup(){
  //Projection Matrix (Orthographic)
  projection = glm::ortho(-(float)Tiny::view.WIDTH*zoom, (float)Tiny::view.WIDTH*zoom, -(float)Tiny::view.HEIGHT*zoom, (float)Tiny::view.HEIGHT*zoom, -800.0f, 500.0f);
  srand(time(NULL));
};

// Event Handler
std::function<void()> eventHandler = [&](){

  if(!Tiny::event.scroll.empty()){

    if(Tiny::event.scroll.back().wheel.y > 0.99){
      zoom /= zoomInc;
      projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -800.0f, 500.0f);
    }
    else if(Tiny::event.scroll.back().wheel.y < -0.99){
      zoom *= zoomInc;
      projection = glm::ortho(-(float)WIDTH*zoom, (float)WIDTH*zoom, -(float)HEIGHT*zoom, (float)HEIGHT*zoom, -800.0f, 500.0f);
    }
    else if(Tiny::event.scroll.back().wheel.x < -0.8){
      rotation += 1.5f;
      camera = glm::rotate(camera, glm::radians(1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else if(Tiny::event.scroll.back().wheel.x > 0.8){
      rotation -= 1.5f;
      camera = glm::rotate(camera, glm::radians(-1.5f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    //Adjust Stuff
    if(rotation < 0.0) rotation = 360.0 + rotation;
    else if(rotation > 360.0) rotation = rotation - 360.0;
    camera = glm::rotate(glm::lookAt(cameraPos, lookPos, glm::vec3(0,1,0)), glm::radians(rotation), glm::vec3(0,1,0));
  }

  //Pause Toggle
  if(!Tiny::event.keys.empty()){
    if(Tiny::event.keys.back().key.keysym.sym == SDLK_p)
      paused = !paused;
    else if(Tiny::event.keys.back().key.keysym.sym == SDLK_a)
      autorotate = !autorotate;
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

        ImGui::Text("Made by Nicholas McDonald");
        ImGui::EndTabItem();

      }

      if(ImGui::BeginTabItem("Growth")){

        if(ImGui::Button("Re-Grow")){
          Branch* newroot = new Branch(tree.root->ratio, tree.root->spread, tree.root->splitsize);
          delete(tree.root);
          tree.root = newroot;
        }

        ImGui::DragFloat("Growth Rate", &tree.rate, 0.01f, 0.0f, 5.0f);
        ImGui::DragFloat("Split Ratio", &tree.root->ratio, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Pass Ratio", &passratio, 0.01f, 0.0f, 1.0f);
        ImGui::Checkbox("Conserve Crossectional Area", &conservearea);
        ImGui::Checkbox("Conserve Diameter Sum", &conservediameter);
        ImGui::DragFloat("Branch Spread", &tree.root->spread, 0.1f, 0.0f, 5.0f);
        ImGui::DragFloat("Split Size", &tree.root->splitsize, 0.1f, 0.1f, 5.0f);

        ImGui::EndTabItem();
      }

      if(ImGui::BeginTabItem("Leaf")){
        ImGui::ColorEdit3("Color", leafcolor);
        ImGui::DragFloat("Opacity", &leafopacity, 0.01f, 0.0f, 1.0f);
        ImGui::DragInt("Count", &leafcount, 1, 0, 30);
        ImGui::DragFloat3("Spread", leafspread, 0.1f, 0.0f, 250.0f);
        ImGui::DragFloat("Size", &leafsize, 0.1f, 0.0f, 25.0f);
        ImGui::Checkbox("Draw", &drawleaf);

        ImGui::EndTabItem();
      }

      if(ImGui::BeginTabItem("Tree")){
        ImGui::ColorEdit3("Fill", treecolor);
        ImGui::ColorEdit3("Wire", wirecolor);
        ImGui::DragFloat("Opacity", &treeopacity, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat2("Scale", treescale, 0.01f, 0.1f, 50.0f);
        ImGui::DragFloat("Taper", &taper, 0.01f, 0.0f, 1.0f);

        ImGui::Checkbox("Draw Tree", &drawtree); ImGui::SameLine();
        ImGui::Checkbox("Draw Wire", &drawwire);

        ImGui::DragInt("Mesh", &ringsize, 1, 3, 12);

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

  ImGui::End();
};

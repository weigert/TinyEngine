//Camera Data
glm::vec3 pos = glm::vec3(20);
glm::vec3 up = glm::vec3(0, 1, 0);
glm::vec3 look = glm::vec3(0,10,0);
glm::mat4 proj;
glm::mat4 view;
float scale = 20.0;

//Pointlight Data
float pointnear = 0.1f;
float pointfar = 60.0f;
std::vector<glm::mat4> views;
glm::mat4 pointproj;
glm::vec3 plightpos = glm::vec3(0, 5, 17);
bool lightupdate = true;

//Pointlight Properties
bool on = true;
float lightcolor[3] = {1.0, 0.9, 0.8};
float brightness = 1.0;
float attenuation[3] = {0.5, 1.0, 5.0};

//Compute Matrices
void setup(){
  proj = glm::ortho(-(float)Tiny::view.WIDTH/scale, (float)Tiny::view.WIDTH/scale, -(float)Tiny::view.HEIGHT/scale, (float)Tiny::view.HEIGHT/scale, -50.0f, 300.0f);
  view = glm::lookAt(pos, look, up);

  pointproj = glm::perspective(glm::radians(90.0f), 1.0f, pointnear, pointfar);
  views.clear();
  views.push_back(pointproj*glm::lookAt(plightpos, plightpos+glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
  views.push_back(pointproj*glm::lookAt(plightpos, plightpos+glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
  views.push_back(pointproj*glm::lookAt(plightpos, plightpos+glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
  views.push_back(pointproj*glm::lookAt(plightpos, plightpos+glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
  views.push_back(pointproj*glm::lookAt(plightpos, plightpos+glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
  views.push_back(pointproj*glm::lookAt(plightpos, plightpos+glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
}

// Event Handler
std::function<void()> eventHandler = [](){

  if(Tiny::event.scroll.posy)
    scale *= 0.9;
  if(Tiny::event.scroll.negy)
    scale /= 0.9;
  if(Tiny::event.scroll.posx)
    pos = glm::rotate(glm::mat4(1),  glm::radians(2.5f), up)*glm::vec4(pos, 1.0);
  if(Tiny::event.scroll.negx)
    pos = glm::rotate(glm::mat4(1), -glm::radians(2.5f), up)*glm::vec4(pos, 1.0);

  setup(); //Recompute (I know this shouldn't be called all the time, whatever lol)
};

//Interface Function
Handle interfaceFunc = [](){
  //Window Size
  ImGui::SetNextWindowSize(ImVec2(480, 260), ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(50, 470), ImGuiCond_Once);

  ImGui::Begin("Point Light Scene Controller", NULL, ImGuiWindowFlags_NoResize);

  ImGui::ColorEdit3("Light Color", lightcolor);
  ImGui::SliderFloat3("Attenuation", attenuation, 0.0f, 25.0f);
  ImGui::Checkbox("On Switch", &on);
  ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f);
  ImGui::SliderFloat3("Position", &plightpos[0], -25.0f, 25.0f);
  lightupdate = true;

  ImGui::End();
};

Model* construct_room(){

  std::vector<float> positions = {
    -1.0, 0.0, -1.0,
    -1.0, 0.0,  1.0,
     1.0, 0.0, -1.0,
     1.0, 0.0,  1.0,
    -1.0, 1.0, -1.0,
    -1.0, 1.0,  1.0,
     1.0, 1.0, -1.0,
     1.0, 1.0,  1.0
  };

  std::vector<int> indices;
  std::vector<float> normals;
  std::vector<float> colors;

  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);

  indices.push_back(1);
  indices.push_back(3);
  indices.push_back(2);

  indices.push_back(4);
  indices.push_back(6);
  indices.push_back(5);

  indices.push_back(5);
  indices.push_back(6);
  indices.push_back(7);

  //Ceiling

  glm::vec3 floorcolor = glm::vec3(0.65, 0.5, 0.3);
  glm::vec3 ceilingcolor = glm::vec3(0.9, 0.9, 0.8);

  for(int i = 0; i < 4; i++){
    normals.push_back(0.0);
    normals.push_back(1.0);
    normals.push_back(0.0);

    colors.push_back(floorcolor.x);
    colors.push_back(floorcolor.y);
    colors.push_back(floorcolor.z);
    colors.push_back(1.0);
  }

  for(int i = 0; i < 4; i++){
    normals.push_back(0.0);
    normals.push_back(-1.0);
    normals.push_back(0.0);

    colors.push_back(ceilingcolor.x);
    colors.push_back(ceilingcolor.y);
    colors.push_back(ceilingcolor.z);
    colors.push_back(1.0);
  }

  float wallA[12] = {
    -1.0, 0.0, -1.0,
     1.0, 0.0, -1.0,
    -1.0, 1.0, -1.0,
     1.0, 1.0, -1.0,
  };

  //Walls
  int P = positions.size()/3;
  indices.push_back(P);
  indices.push_back(P+1);
  indices.push_back(P+2);
  indices.push_back(P+1);
  indices.push_back(P+3);
  indices.push_back(P+2);

  for(int i = 0; i < 12; i++)
    positions.push_back(wallA[i]);

  for(int i = 0; i < 4; i++){
    normals.push_back(0.0);
    normals.push_back(0.0);
    normals.push_back(1.0);

    colors.push_back(ceilingcolor.x);
    colors.push_back(ceilingcolor.y);
    colors.push_back(ceilingcolor.z);
    colors.push_back(1.0);
  }

  float wallB[12] = {
    -1.0, 0.0, 1.0,
     1.0, 0.0, 1.0,
    -1.0, 1.0, 1.0,
     1.0, 1.0, 1.0,
  };

  //Walls
  P = positions.size()/3;
  indices.push_back(P);
  indices.push_back(P+2);
  indices.push_back(P+1);
  indices.push_back(P+1);
  indices.push_back(P+2);
  indices.push_back(P+3);

  for(int i = 0; i < 12; i++)
    positions.push_back(wallB[i]);

  for(int i = 0; i < 4; i++){
    normals.push_back( 0.0);
    normals.push_back( 0.0);
    normals.push_back(-1.0);

    colors.push_back(ceilingcolor.x);
    colors.push_back(ceilingcolor.y);
    colors.push_back(ceilingcolor.z);
    colors.push_back(1.0);
  }


  float wallC[12] = {
    -1.0, 0.0,-1.0,
    -1.0, 0.0, 1.0,
    -1.0, 1.0,-1.0,
    -1.0, 1.0, 1.0,
  };

  //Walls
  P = positions.size()/3;
  indices.push_back(P);
  indices.push_back(P+2);
  indices.push_back(P+1);
  indices.push_back(P+1);
  indices.push_back(P+2);
  indices.push_back(P+3);

  for(int i = 0; i < 12; i++)
    positions.push_back(wallC[i]);

  for(int i = 0; i < 4; i++){
    normals.push_back( 1.0);
    normals.push_back( 0.0);
    normals.push_back( 0.0);

    colors.push_back(ceilingcolor.x);
    colors.push_back(ceilingcolor.y);
    colors.push_back(ceilingcolor.z);
    colors.push_back(1.0);
  }

  float wallD[12] = {
     1.0, 0.0,-1.0,
     1.0, 0.0, 1.0,
     1.0, 1.0,-1.0,
     1.0, 1.0, 1.0,
  };

  //Walls
  P = positions.size()/3;
  indices.push_back(P);
  indices.push_back(P+1);
  indices.push_back(P+2);
  indices.push_back(P+1);
  indices.push_back(P+3);
  indices.push_back(P+2);

  for(int i = 0; i < 12; i++)
    positions.push_back(wallD[i]);

  for(int i = 0; i < 4; i++){
    normals.push_back(-1.0);
    normals.push_back( 0.0);
    normals.push_back( 0.0);

    colors.push_back(ceilingcolor.x);
    colors.push_back(ceilingcolor.y);
    colors.push_back(ceilingcolor.z);
    colors.push_back(1.0);
  }

  Model* model = new Model({"in_Position", "in_Normal", "in_Color"});
  model->bind<glm::vec3>("in_Position", new Buffer(positions));
  model->bind<glm::vec3>("in_Normal", new Buffer(normals));
  model->bind<glm::vec4>("in_Color", new Buffer(colors));
  model->SIZE = positions.size()/3;
  model->index(new Buffer(indices));
  return model;

}

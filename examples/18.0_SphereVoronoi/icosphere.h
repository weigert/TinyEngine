#define PI 3.14159265f

struct Icosphere: Model {

  std::vector<glm::vec3> positions;
  std::vector<glm::uvec3> indices;
  Buffer pos, ind;

  Icosphere():Model({"in_Position", "in_Tex"}){

    build();
    split();

    pos.fill(positions);
    ind.fill(indices);

    bind<glm::vec3>("in_Position", &pos);
    index(&ind);
    SIZE = indices.size()*3;

  }

  void build();
  void split();
  void sort(){
    std::sort(indices.begin(), indices.end(), [&](const glm::uvec3& a, const glm::uvec3& b){
      glm::vec3 ap = cam::vp*glm::vec4((positions[a.x] + positions[a.y] + positions[a.z])/3.0f, 1.0);
      glm::vec3 bp = cam::vp*glm::vec4((positions[b.x] + positions[b.y] + positions[b.z])/3.0f, 1.0);
      return ap.z > bp.z;
    });
    ind.fill(indices);
  }

};

void Icosphere::build(){

  const float r = 1.0f;
  const float a = 72.0f/360.0f*2.0f*PI;

  //Helper Methods
  auto addpos = [&](float a, float b, float c){
    positions.emplace_back(a, b, c);
  };

  //For a Triangles
  auto triangle = [&](int a, int b, int c){
    indices.emplace_back(a, b, c);
  };

  //Top Cap
  addpos(0, r, 0);

  for(int i = 0; i < 5; i++){ //Top
    float x1 = r*cos(atan(0.5))*cos(a*i);
    float y1 = r*cos(atan(0.5))*sin(a*i);
    float z1 = r*sin(atan(0.5));
    addpos(x1,z1,y1);
  }

  for(int i = 0; i < 5; i++){ //Bottom
    float x2 = r*cos(atan(0.5))*cos(a*i+a/2.0f);
    float y2 = r*cos(atan(0.5))*sin(a*i+a/2.0f);
    float z2 = -r*sin(atan(0.5));
    addpos(x2,z2,y2);
  }

  //Bottom Cap
  addpos(0,-r,0);

  //Top Triangles
  triangle(1,0,2);
  triangle(2,0,3);
  triangle(3,0,4);
  triangle(4,0,5);
  triangle(5,0,1);

  //Bottom Triangles
  triangle(6,7,11);
  triangle(7,8,11);
  triangle(8,9,11);
  triangle(9,10,11);
  triangle(10,6,11);

  //Connecting Triangles (Bottom)
  triangle(6,2,7);
  triangle(7,3,8);
  triangle(8,4,9);
  triangle(9,5,10);
  triangle(10,1,6);

  //Connecting Triangles (Top)
  triangle(2,3,7);
  triangle(1,2,6);
  triangle(3,4,8);
  triangle(4,5,9);
  triangle(5,1,10);

}

void Icosphere::split(){

  std::vector<glm::uvec3> newind;

  auto addpos = [&](glm::vec3 p){
    positions.push_back(p);
  };

  auto triangle = [&](int a, int b, int c){
    newind.emplace_back(a, b, c);
  };

  auto splittriangle = [&](int i){

    //Indicies of the old triangle
    GLuint k1 = indices[i][0];
    GLuint k2 = indices[i][1];
    GLuint k3 = indices[i][2];

    //Positions of the old triangle
    glm::vec3 a = positions[k1];
    glm::vec3 b = positions[k2];
    glm::vec3 c = positions[k3];

    //Starting Indices
    int NI = positions.size();

    //Compute Split Points
    glm::vec3 d = normalize(0.5f*(a+b));
    glm::vec3 e = normalize(0.5f*(b+c));
    glm::vec3 f = normalize(0.5f*(c+a));

    //Add New Positions to Model
    addpos(d); addpos(e); addpos(f);

    triangle(k1, NI+0, NI+2);
    triangle(k2, NI+1, NI+0);
    triangle(k3, NI+2, NI+1);
    triangle(NI+0, NI+1, NI+2);

  };

  for(size_t j = 0; j < 6; j++){
    newind.clear();
    for(int i = 0; i < indices.size(); i++)   //Loop over Old Triangles
      splittriangle(i);
    indices = newind;
  }

}

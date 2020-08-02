/*
  Model Loading Namespace from Object Files
*/

namespace obj{

  //Get Colors from material list
  std::unordered_map<std::string, glm::vec3> materials(std::string file){
    std::unordered_map<std::string, glm::vec3> mat;

    //Open the File
    std::ifstream in(file+".mtl", std::ios::in);
    if(in){

      std::string line;
      std::string matname;
      while (getline(in, line)){
        if(line[0] == '#') continue;  //Ignore Comments

        if(line.substr(0, 6) == "newmtl")
          matname = line.substr(7);

        else if(line.substr(0, 2) == "Kd"){
          float R, G, B;
          int m = std::sscanf(line.substr(3).c_str(), "%f %f %f\n", &R, &G, &B);
          if(m != 3) std::cout<<"Material Color Data was not read correctly."<<std::endl;
          else mat[matname] = glm::vec3(R, G, B);
        }
      }
      in.close();
    }
    else std::cout<<"Failed to open file "<<file<<std::endl;
    return mat;
  }

  //Construct a Model from a .Obj File
  std::function<void(Model*, std::string)> load = [](Model* h, std::string file){
    h->indexed = false;
    std::unordered_map<std::string, glm::vec3> mat = materials(file);

    //Temporary Buffers
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

    std::ifstream in(file+".obj", std::ios::in);

    if(!in){
      std::cout<<"Failed to open file "<<file<<std::endl;
      return;
    }

    glm::vec3 color = glm::vec3(1.0);

    std::string line;
    while (getline(in, line)){

      //Ignore Comments
      if(line[0] == '#') continue;

      //Extract Vertex Information
      if(line.substr(0,2) == "v "){
        std::istringstream s(line.substr(2));
        glm::vec3 v;
        s >> v.x; s >> v.y; s >> v.z;
        vertices.push_back(v);
      }

      //Extract Normal Information
      else if(line.substr(0,3) == "vn "){
        std::istringstream s(line.substr(3));
        glm::vec3 n;
        s >> n.x; s >> n.y; s >> n.z;
        normals.push_back(n);
      }

      else if(line.substr(0, 6) == "usemtl")
        color = mat[line.substr(7)];

      //Map Index Information
      else if(line.substr(0,2) == "f "){
        //Parse Face Data
        unsigned int vI[3], uI[3], nI[3];
        int m = std::sscanf(line.substr(2).c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vI[0], &uI[0], &nI[0], &vI[1], &uI[1], &nI[1], &vI[2], &uI[2], &nI[2]);
        if(m != 9){
          std::cout<<"Face data could not be read correctly."<<std::endl;
          return;
        }

        //Push Face Data
        h->indices.push_back(h->positions.size()/3);

        for(int i = 0; i < 3; i++){
          h->positions.push_back(vertices[vI[i]-1].x);
          h->positions.push_back(vertices[vI[i]-1].y);
          h->positions.push_back(vertices[vI[i]-1].z);
          h->normals.push_back(normals[nI[i]-1].x);
          h->normals.push_back(normals[nI[i]-1].y);
          h->normals.push_back(normals[nI[i]-1].z);
          h->colors.push_back(color.x);
          h->colors.push_back(color.y);
          h->colors.push_back(color.z);
          h->colors.push_back(1.0);
        }
      }
    }

    in.close();
  };

}

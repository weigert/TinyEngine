#ifndef TINYENGINE_UTIL_OBJECT
#define TINYENGINE_UTIL_OBJECT

#include <TinyEngine/Model>

#include <fstream>
#include <sstream>

namespace Tiny {

//! Object is a Model type for loading .obj and .mtl
//! files directly into a renderable object.
//!
struct Object: Model {

  using Model::render;

  Object(const std::string filename):
  Model({"in_Position", "in_Normal", "in_Color"}){
    this->load_material(filename + ".mtl");
    this->load_geometry(filename + ".obj");
    this->bind<glm::vec3>("in_Position", b_positions);
    this->bind<glm::vec3>("in_Normal", b_normals);
    this->bind<glm::vec3>("in_Color", b_colors);
  }

  void load_geometry(const std::string filename); //!< Load the Geometry from ".obj"
  void load_material(const std::string filename); //!< Load the Materials from ".mtl"

private:
  std::unordered_map<std::string, glm::vec3> materials;
  Buffer b_positions;
  Buffer b_normals;
  Buffer b_colors;
};

// Implementations

void Object::load_material(const std::string filename){

  std::ifstream in(filename, std::ios::in);
  if(!in){
    std::cout<<"Failed to open file "<<filename<<std::endl;
    return;
  }

  std::string line;
  std::string matname;

  while(std::getline(in, line)){
    if(line[0] == '#') continue;  //Ignore Comments
    if(line.substr(0, 6) == "newmtl")
      matname = line.substr(7);
    else if(line.substr(0, 2) == "Kd"){
      float R, G, B;
      int m = sscanf(line.substr(3).c_str(), "%f %f %f\n", &R, &G, &B);
      if(m != 3) std::cout<<"Material Color Data was not read correctly."<<std::endl;
      else materials[matname] = glm::vec3(R, G, B);
    }
  }
  in.close();
}

void Object::load_geometry(const std::string filename){

  std::vector<glm::vec3> vbuf;
  std::vector<glm::vec3> nbuf;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec4> colors;

  std::ifstream in(filename, std::ios::in);
  if(!in){
    std::cout<<"Failed to open file "<<filename<<std::endl;
  }

  glm::vec3 color = glm::vec3(1.0);
  bool vt = false;

  std::string line;
  while(std::getline(in, line)){

    if(line[0] == '#') continue;  //Ignore Comments

    else if(line.substr(0,2) == "v "){  //Vertex Data
      std::istringstream s(line.substr(2));
      glm::vec3 v;
      s >> v.x; s >> v.y; s >> v.z;
      vbuf.push_back(v);
    }

    else if(line.substr(0,3) == "vt"){  //Texture Data - Not Handled
      vt = true;
    }

    else if(line.substr(0,3) == "vn "){ //Normal Data
      std::istringstream s(line.substr(3));
      glm::vec3 n;
      s >> n.x; s >> n.y; s >> n.z;
      nbuf.push_back(n);
    }

    else if(line.substr(0, 6) == "usemtl"){
      color = materials[line.substr(7)];
      continue;
    }

    //Map Index Information
    else if(line.substr(0,2) == "f "){
      unsigned int vI[3], uI[3], nI[3];
      if(nbuf.empty() && !vt){ //Simple Format
        int m = sscanf(line.substr(2).c_str(), "%d %d %d\n", &vI[0], &vI[1], &vI[2]);
        if(m != 3){
          std::cout<<"Face data could not be read correctly."<<std::endl;
          return;
        }
      }
      else { //Parse Face Data Normally
        int m = sscanf(line.substr(2).c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vI[0], &uI[0], &nI[0], &vI[1], &uI[1], &nI[1], &vI[2], &uI[2], &nI[2]);
        if(m != 9) {
          m = sscanf(line.substr(2).c_str(), "-%d//-%d -%d//-%d -%d//-%d\n", &vI[0], &nI[0], &vI[1], &nI[1], &vI[2], &nI[2]);
          if(m != 6){
              std::cout<<"Face data could not be read correctly."<<std::endl;
              return;
          }
        }
      }

      for(int i = 0; i < 3; i++){
        positions.push_back(vbuf[vI[i]-1]);
      }
      if(!normals.empty())
        for(int i = 0; i < 3; i++){
          normals.push_back(nbuf[nI[i]-1]);
        }
      else{
        glm::vec3 a = vbuf[vI[0]-1];
        glm::vec3 b = vbuf[vI[1]-1];
        glm::vec3 c = vbuf[vI[2]-1];
        glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
        for(int i = 0; i < 3; i++){
          normals.push_back(n);
        }
      }
      for(int i = 0; i < 3; i++){
        colors.push_back(glm::vec4(color, 1.0));
      }
    }
  }

  in.close();

  b_positions.set(positions);
  b_normals.set(normals);
  b_colors.set(colors);

}

};  // end of namespace Tiny

#endif

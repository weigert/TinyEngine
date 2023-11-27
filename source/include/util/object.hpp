#ifndef TINYENGINE_HELPER_OBJECT
#define TINYENGINE_HELPER_OBJECT

#include <fstream>
#include <sstream>

namespace obj {
using namespace std;

  //Get Colors from material list
  unordered_map<string, glm::vec3> materials(string file){
    unordered_map<string, glm::vec3> mat;

    //Open the File
    ifstream in(file+".mtl", ios::in);
    if(in){

      string line;
      string matname;
      while (getline(in, line)){
        if(line[0] == '#') continue;  //Ignore Comments

        if(line.substr(0, 6) == "newmtl")
          matname = line.substr(7);

        else if(line.substr(0, 2) == "Kd"){
          float R, G, B;
          int m = sscanf(line.substr(3).c_str(), "%f %f %f\n", &R, &G, &B);
          if(m != 3) cout<<"Material Color Data was not read correctly."<<endl;
          else mat[matname] = glm::vec3(R, G, B);
        }
      }
      in.close();
    }
    else cout<<"Failed to open file "<<file<<".mtl"<<endl;
    return mat;
  }

  Tiny::Model* load(string file){

    unordered_map<string, glm::vec3> mat = materials(file);

    vector<glm::vec3> vbuf;
    vector<glm::vec3> nbuf;
    vector<glm::vec3> positions;
    vector<glm::vec3> normals;
    vector<glm::vec4> colors;

    ifstream in(file+".obj", ios::in);
    if(!in){
      cout<<"Failed to open file "<<file<<".obj"<<endl;
      return NULL;
    }

    glm::vec3 color = glm::vec3(1.0);
    bool vt = false;

    string line;
    while (getline(in, line)){

      if(line[0] == '#') continue;  //Ignore Comments

      else if(line.substr(0,2) == "v "){  //Vertex Data
        istringstream s(line.substr(2));
        glm::vec3 v;
        s >> v.x; s >> v.y; s >> v.z;
        vbuf.push_back(v);
      }

      else if(line.substr(0,3) == "vt"){  //Texture Data - Not Handled
        vt = true;
      }

      else if(line.substr(0,3) == "vn "){ //Normal Data
        istringstream s(line.substr(3));
        glm::vec3 n;
        s >> n.x; s >> n.y; s >> n.z;
        nbuf.push_back(n);
      }

      else if(line.substr(0, 6) == "usemtl"){
        color = mat[line.substr(7)];
        continue;
      }

      //Map Index Information
      else if(line.substr(0,2) == "f "){
        unsigned int vI[3], uI[3], nI[3];
        if(nbuf.empty() && !vt){ //Simple Format
          int m = sscanf(line.substr(2).c_str(), "%d %d %d\n", &vI[0], &vI[1], &vI[2]);
          if(m != 3){
            cout<<"Face data could not be read correctly."<<endl;
            return NULL;
          }
        }
        else { //Parse Face Data Normally
          int m = sscanf(line.substr(2).c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vI[0], &uI[0], &nI[0], &vI[1], &uI[1], &nI[1], &vI[2], &uI[2], &nI[2]);
          if(m != 9) {
            m = sscanf(line.substr(2).c_str(), "-%d//-%d -%d//-%d -%d//-%d\n", &vI[0], &nI[0], &vI[1], &nI[1], &vI[2], &nI[2]);
            if(m != 6){
                cout<<"Face data could not be read correctly."<<endl;
                return NULL;
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

    //Construct the Model with Buffers
    Tiny::Model* model = new Tiny::Model({"in_Position", "in_Normal", "in_Color"});
    model->bind("in_Position", new Tiny::Buffer<glm::vec3>(positions), true);
    model->bind("in_Normal", new Tiny::Buffer<glm::vec3>(normals), true);
    model->bind("in_Color", new Tiny::Buffer<glm::vec4>(colors), true);
    model->SIZE = positions.size();
    return model;

  }

}

#endif
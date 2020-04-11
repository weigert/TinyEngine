#pragma once
class Shader{
public:

  //Constructor
  Shader(std::string vs, std::string fs, slist _list){
    setup(vs, fs);  //Setup the Shader
    for(auto &n : _list)
      addInput(&n-_list.begin(), n); //Add all Properties
  };

  ~Shader(){
    cleanup();
  }

  GLuint shaderProgram;   //Shader Program ID
  GLuint vertexShader, fragmentShader;

  //High level Usage
  void setup(std::string vs, std::string fs);
  void addInput(int pos, std::string attrName);
  void cleanup();
  int  addProgram(std::string fileName, GLenum shaderType);  //General Shader Addition
  std::string readGLSLFile(std::string fileName, int32_t &size); //Read File
  void compile(GLuint shader);  //Compile and Add File
  void link();                  //Link the entire program
  void use();                   //Use the program

  //Set Uniforms
  void setBool(std::string name, bool value);
  void setInt(std::string name, int value);
  void setFloat(std::string name, float value);
  void setVec2(std::string name, const glm::vec2 vec);
  void setVec3(std::string name, const glm::vec3 vec);
  void setVec4(std::string name, const glm::vec4 vec);
  void setMat3(std::string name, const glm::mat3 mat);
  void setMat4(std::string name, const glm::mat4 mat);
};

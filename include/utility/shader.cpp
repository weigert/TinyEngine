#include "shader.h"

void Shader::setup(std::string vs, std::string fs){
  //Generate our Shader
  shaderProgram = glCreateProgram();

  //Current Path
  boost::filesystem::path data_dir(boost::filesystem::current_path());

  //Add the Shader Programs from File
  vertexShader   = addProgram((data_dir/vs).string(), GL_VERTEX_SHADER);
  fragmentShader = addProgram((data_dir/fs).string(), GL_FRAGMENT_SHADER);

  //Link the Shader
  link();
}

int Shader::addProgram(std::string fileName, GLenum shaderType){
  //Read Shader Program from Source
  char* src;
  int32_t size;
  std::string result = readGLSLFile(fileName, size);
  src = const_cast<char*>(result.c_str());

  //Create and Compile Shader
  int shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &src, &size);
  compile(shaderID);

  return shaderID;
}

void Shader::addInput(int pos, std::string attrName){
  glBindAttribLocation(shaderProgram, pos, attrName.c_str());
}

void Shader::compile(GLuint shader){
  glCompileShader(shader);

  //Bug Checking
  int wasCompiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &wasCompiled );

  if (wasCompiled == 0){
    std::cout<<"Error"<<std::endl;
    int maxLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    char* shaderInfoLog = new char[maxLength];
    glGetShaderInfoLog(shader, maxLength, &maxLength, shaderInfoLog);
    std::cout << "Linker error message : " << shaderInfoLog << std::endl;
  }
  else{
    glAttachShader(shaderProgram, shader);
  }
}

void Shader::link(){
  glLinkProgram(shaderProgram);
  int isLinked;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == false){
    int maxLength;
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

    // Get shader info log
    char* shaderProgramInfoLog = new char[maxLength];
    glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);
    std::cout << "Linker error message: " << shaderProgramInfoLog << std::endl;

    delete shaderProgramInfoLog;
  }
}

void Shader::use(){
  glUseProgram(shaderProgram);
}

std::string Shader::readGLSLFile(std::string fileName, int32_t &size){
  //Load the stuff
  //Fragment Shader
  std::ifstream t;
  std::string fileContent;
  //Open the file
  t.open(fileName);
  if(t.is_open()){
    std::stringstream buffer;
    buffer << t.rdbuf();
    fileContent = buffer.str();
  }
  else{
    std::cout<<"File opening failed"<<std::endl;
  }
  t.close();
  //Load into the variables
  size = fileContent.length();
  return fileContent;
}

void Shader::cleanup(){
  //Delete the Objects
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragmentShader);
  glDeleteShader(vertexShader);
}

/* Uniform Setters */

void Shader::setBool(std::string name, bool value){
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setInt(std::string name, int value){
  ///Set the Uniform
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setFloat(std::string name, float value){
  ///Set the Uniform
  glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setVec2(std::string name, const glm::vec2 vec){
  glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}

void Shader::setVec3(std::string name, const glm::vec3 vec){
  glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}

void Shader::setVec4(std::string name, const glm::vec4 vec){
  glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}

void Shader::setMat3(std::string name, const glm::mat3 mat){
  glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(std::string name, const glm::mat4 mat){
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

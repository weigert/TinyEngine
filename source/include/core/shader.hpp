#ifndef TINYENGINE_CORE_SHADER
#define TINYENGINE_CORE_SHADER

#include <TinyEngine/Buffer>
#include <TinyEngine/Texture>

#include <filesystem>
#include <sstream>
#include <iostream>

namespace Tiny {

//! ShaderStage represents a single stage
//! of a multi-stage shader program.
//!
struct ShaderStage {

  ShaderStage(GLenum shaderType){
    this->_index = glCreateShader(shaderType);
  }

  ShaderStage(GLenum shaderType, const std::string filename):
  ShaderStage(shaderType){
    this->setup(filename);
  }

  ~ShaderStage(){
    glDeleteShader(_index);
  }

  void setup(const std::string filename){
    this->load(filename);
    this->compile();
  }

  //! Load the GLSL Source
  void load(const std::string filename){
    int32_t size;
    const std::string result = readGLSLFile(filename, size);
    char* src = const_cast<char*>(result.c_str());
    glShaderSource(this->index(), 1, &src, &size);
  }

  //! Compile the GLSL Source
  void compile(){
    glCompileShader(this->index());
    glGetShaderiv(this->index(), GL_COMPILE_STATUS, &_valid);
  }

  static std::string readGLSLFile(const std::string filename, int32_t &size);

  inline const uint32_t valid() const { return this->_valid; }
  inline const uint32_t index() const { return this->_index; }

private:
  GLint _valid;    //!< ShaderStage Validity Status
  uint32_t _index; //!< Index of the Shader Stage
};

struct Shader {
private:

  Shader():
  vertexShader(GL_VERTEX_SHADER),
  fragmentShader(GL_FRAGMENT_SHADER),
  geometryShader(GL_GEOMETRY_SHADER){
    _index = glCreateProgram();
  }

public:

  ~Shader(){
    glDeleteProgram(this->index());
  }

  Shader(const std::vector<std::string> shaders, const std::vector<std::string> in):Shader(){

    if(shaders.size() == 2){
      
      vertexShader.setup(shaders[0]);
      if(!vertexShader.valid()){
        error(vertexShader.index(), true, shaders[0]);
        return;
      } else {
        glAttachShader(this->index(), vertexShader.index());
      }

      fragmentShader.setup(shaders[1]);
      if(!fragmentShader.valid()){
        error(fragmentShader.index(), true, shaders[1]);
        return;
      } else {
        glAttachShader(this->index(), fragmentShader.index());
      }

    }


    else if(shaders.size() == 3){
      vertexShader.setup(shaders[0]);
      if(!vertexShader.valid()){
        error(vertexShader.index(), true, shaders[0]);
        return;
      } else {
        glAttachShader(this->index(), vertexShader.index());
      }

      geometryShader.setup(shaders[1]);
      if(!geometryShader.valid()){
        error(geometryShader.index(), true, shaders[1]);
        return;
      } else {
        glAttachShader(this->index(), geometryShader.index());
      }

      fragmentShader.setup(shaders[2]);
      if(!fragmentShader.valid()){
        error(fragmentShader.index(), true, shaders[2]);
        return;
      } else {
        glAttachShader(this->index(), fragmentShader.index());
      }
    }
    else std::cout<<"Number of shaders not recognized."<<std::endl;

    for(int i = 0; i < in.size(); i++)
      glBindAttribLocation(this->index(), i, in[i].c_str());

    this->link(shaders[0]);
  }

  void link(const std::string fileName){
    glLinkProgram(this->index());
    glGetProgramiv(this->index(), GL_LINK_STATUS, &_valid);
    if(!_valid) error(this->index(), false, fileName);
  }

  void use(){
    glUseProgram(this->index());
    _textures = 0;
  }

  void error(GLuint s, bool t, std::string fileName){
    int m;
    if(t) glGetShaderiv(s, GL_INFO_LOG_LENGTH, &m);
    else glGetProgramiv(s, GL_INFO_LOG_LENGTH, &m);
    char* l = new char[m];
    if(t) glGetShaderInfoLog(s, m, &m, l);
    else glGetProgramInfoLog(s, m, &m, l);
    std::cout<<"Linker Error ("<<fileName<<"): "<<l<<std::endl;
    delete[] l;
  }

  inline const uint32_t index() const { return this->_index; }

  template<typename T> void uniform(std::string name, const T u);
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]);
  template<typename T> void texture(std::string name, const T& t);

private:
  GLint _valid;            //!< Validity of Linked Program
  uint32_t _index;            //!< Index of the Multi-Stage Program
  uint32_t _textures;         //!< Number of Bound Textures
  ShaderStage vertexShader;
  ShaderStage geometryShader;
  ShaderStage fragmentShader;
};

/*
// Then, a shader is a composition of multiple shaderstages?

class ShaderBase {
public:

  static std::unordered_map<std::string, GLuint> sbpi; //Shader Binding Point Index

  static void ssbo(std::string name);                  //Define an SSBO by name
  static void ssbo(std::vector<std::string> names);    //Define a list of SSBOs
  void interface(std::string name);                    //Add SSBO to permitted interface blocks
  void interface(std::vector<std::string> names);      //Add a list of buffers to interface
  static void bind(std::string name, Buffer* buf);

};


std::unordered_map<std::string, GLuint> Shader::sbpi; //Shader Binding Point Index

void Shader::link(std::string fileName){

}



void Shader::ssbo(std::string name){
  if(sbpi.find(name) != sbpi.end()) return; //Named Binding Point Exists
  sbpi[name] = sbpi.size();
}

void Shader::ssbo(std::vector<std::string> names){
  for(auto& l: names) ssbo(l);
}

void Shader::interface(std::string name){
  ssbo(name); //Make sure binding point exists
  glShaderStorageBlockBinding(program, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, name.c_str()), sbpi[name]);
}

void Shader::interface(std::vector<std::string> names){
  for(auto& l: names) interface(l);
}

void Shader::bind(std::string name, Buffer* buf){
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf->index());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sbpi[name], buf->index());
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

*/

template<typename T>
void Shader::uniform(std::string name, T u){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<typename T, size_t N>
void Shader::uniform(std::string name, const T (&u)[N]){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<> void Shader::uniform(std::string name, const bool u){
  glUniform1i(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void Shader::uniform(std::string name, const int u){
  glUniform1i(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void Shader::uniform(std::string name, const float u){
  glUniform1f(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void Shader::uniform(std::string name, const double u){ //GLSL Intrinsically Single Precision
  glUniform1f(glGetUniformLocation(this->index(), name.c_str()), (float)u); }

template<> void Shader::uniform(std::string name, const glm::vec2 u){
  glUniform2fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::ivec2 u){
  glUniform2iv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::vec3 u){
  glUniform3fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const float (&u)[3]){
  glUniform3fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const float (&u)[4]){
  glUniform4fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::vec4 u){
  glUniform4fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void Shader::uniform(std::string name, const glm::mat3 u){
  glUniformMatrix3fv(glGetUniformLocation(this->index(), name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void Shader::uniform(std::string name, const glm::mat4 u){
  glUniformMatrix4fv(glGetUniformLocation(this->index(), name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void Shader::uniform(std::string name, const std::vector<glm::mat4> u){
  glUniformMatrix4fv(glGetUniformLocation(this->index(), name.c_str()), u.size(), GL_FALSE, &u[0][0][0]); }

template<typename T>
void Shader::texture(std::string name, const T& t){
  glActiveTexture(GL_TEXTURE0 + _textures);
  t.operator()();
  uniform(name, _textures++);
}

  /*
struct Shader {
public:


  template<typename... Args>
  Shader(std::vector<std::string> shaders, std::vector<std::string> in):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    for(int i = 0; i < in.size(); i++)
      glBindAttribLocation(program, i, in[i].c_str());
    link(shaders[0]);                        //Link the shader program!
  }

  Shader(std::vector<std::string> shaders):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    link(shaders[0]);                             //Link the shader program!
  }

  Shader(std::vector<std::string> shaders, std::vector<std::string> in):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    for(int i = 0; i < in.size(); i++)
      glBindAttribLocation(program, i, in[i].c_str());
    link(shaders[0]);                             //Link the shader program!
  }

  Shader(std::vector<std::string> shaders, std::vector<std::string> in, std::vector<std::string> buf):Shader(shaders, in){
    ssbo(buf);
  }

  ~Shader(){
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
    glDeleteShader(vertexShader);
  }

  void setup(std::vector<std::string> shaders){
    std::vector<std::string> s = shaders;

    if(s.size() == 2){
      vertexShader   = addProgram(s[0], GL_VERTEX_SHADER);
      fragmentShader = addProgram(s[1], GL_FRAGMENT_SHADER);
    }
    else if(s.size() == 3){
      vertexShader   = addProgram(s[0], GL_VERTEX_SHADER);
      geometryShader = addProgram(s[1], GL_GEOMETRY_SHADER);
      fragmentShader = addProgram(s[2], GL_FRAGMENT_SHADER);
    }
    else std::cout<<"Number of shaders not recognized."<<std::endl;
  }


private:
  GLuint vertexShader, geometryShader, fragmentShader;

};

class Compute : public ShaderBase {
private:

  GLuint computeShader;

public:

  Compute(std::string shader):ShaderBase(),
  computeShader(addProgram(shader, GL_COMPUTE_SHADER)){
    link(shader);
  }

  Compute(std::string shader, std::vector<std::string> buf):Compute(shader){
    ssbo(buf);
  }

  ~Compute(){
    glDeleteShader(computeShader);
  }

  void dispatch(int x = 1, int y = 1, int z = 1, bool block = true){
    glDispatchCompute(x, y, z);
    if(block) glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

  static void limits(){
    const std::function<int(GLenum)> getInt = [](GLenum E){
      int i; glGetIntegerv(E, &i); return i;
    };
    std::cout<<"Max. SSBO: "<<getInt(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS)<<std::endl;
    std::cout<<"Max. SSBO Block-Size: "<<getInt(GL_MAX_SHADER_STORAGE_BLOCK_SIZE)<<std::endl;
    std::cout<<"Max. Compute Shader Storage Blocks: "<<getInt(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS)<<std::endl;
    std::cout<<"Max. Shared Storage Size: "<<getInt(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE)<<std::endl;

    int m;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &m);
    std::cout<<"Max. Work Groups: "<<m<<std::endl;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &m);
    std::cout<<"Max. Local Size: "<<m<<std::endl;

  }

};
  */

// Implementation

std::string ShaderStage::readGLSLFile(std::string file, int32_t &size){

  std::filesystem::path local_dir(file);
  local_dir = local_dir.parent_path();

  FILE* stream;
  std::string fileContent;
  std::string line;

  stream = fopen(file.c_str(), "r");
  if(stream != NULL){
    std::stringstream buffer;
    char* cline = NULL; size_t len = 0; ssize_t nread;
    while ((nread = getline(&cline, &len, stream)) != -1) {
      line.assign(cline, nread);
      if(line.substr(0, 9) == "#include "){
        int includesize = 0;
        line.assign(cline, nread-1);  //Remove Newline
        buffer << readGLSLFile((local_dir/line.substr(9)).string(), includesize);
      }
      else buffer << line;
    }
    fileContent = buffer.str();
  }
  else{
    std::cout<<"File opening \""<<file<<"\" failed"<<std::endl;
    return "";
  }
  fclose(stream);

  size = fileContent.length();  //Set the Size
  return fileContent;
}

} // end of namespace Tiny

#endif

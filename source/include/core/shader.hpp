#ifndef TINYENGINE_CORE_SHADER
#define TINYENGINE_CORE_SHADER

#include <TinyEngine/Buffer>
#include <TinyEngine/Texture>

#include <filesystem>
#include <sstream>
#include <iostream>

namespace Tiny {

namespace {

//! Read a GLSL File w. Include Directives
std::string readGLSLFile(const std::string file, int32_t& size){

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

void printShaderError(uint32_t index, const std::string filename){
  int m;
  glGetShaderiv(index, GL_INFO_LOG_LENGTH, &m);
  char* l = new char[m];
  glGetShaderInfoLog(index, m, &m, l);
  std::cout<<"Shader Compiler Error ("<<filename<<"): "<<l<<std::endl;
  delete[] l;
}

void printProgramError(uint32_t index){
  int m;
  glGetProgramiv(index, GL_INFO_LOG_LENGTH, &m);
  char* l = new char[m];
  glGetProgramInfoLog(index, m, &m, l);
  std::cout<<"Shader Linker Error: "<<l<<std::endl;
  delete[] l;
}

}

//! ShaderStage represents a single stage
//! of a multi-stage shader program.
//!
struct ShaderStage {

  ShaderStage(GLenum shaderType){
    this->_index = glCreateShader(shaderType);
  }

  ~ShaderStage(){
    glDeleteShader(this->index());
  }

  ShaderStage(GLenum shaderType, const std::string filename):
  ShaderStage(shaderType){
    this->load(filename);
  }

  //! Load the GLSL Source into the Shader Stage by Filename,
  //! compile the source and handle compilation errors.
  void load(const std::string filename){
    int32_t size;
    const std::string result = readGLSLFile(filename, size);
    char* src = const_cast<char*>(result.c_str());
    glShaderSource(this->index(), 1, &src, &size);
    glCompileShader(this->index());
    glGetShaderiv(this->index(), GL_COMPILE_STATUS, &_valid);
    if(!this->valid()) printShaderError(this->index(), filename);
  }

  inline const uint32_t valid() const { return this->_valid; }  //!< Check Shader Validity
  inline const uint32_t index() const { return this->_index; }  //!< Retrieve Shader Stage Index

private:
  GLint _valid;    //!< ShaderStage Validity Status
  uint32_t _index; //!< Index of the Shader Stage
};

//! ShaderProgram is an executable combination of multiple shader stages.
//!
//! An arbitrary set of ShaderStage can be attached to a ShaderProgram.
//! ShaderProgram provides a templated interface for providing uniforms
//! and textures to the shader stages.
//!
//! Additionally, a ShaderProgram can have ShaderStorageBufferObjects
//! attached as interface blocks, for non-texture generic data.
//!
struct ShaderProgram {

  ShaderProgram(){
    this->_index = glCreateProgram();
  }

  ~ShaderProgram(){
    glDeleteProgram(this->index());
  }

  void attributes(const std::vector<std::string> attributes){
    for(int i = 0; i < attributes.size(); i++)
      glBindAttribLocation(this->index(), i, attributes[i].c_str());
  }

  void attach(ShaderStage& stage){
    if(stage.valid())
      glAttachShader(this->index(), stage.index());
  }

  void link(){
    glLinkProgram(this->index());
    glGetProgramiv(this->index(), GL_LINK_STATUS, &_valid);
    if(!this->valid()) printProgramError(this->index());
  }

  void use(){
    glUseProgram(this->index());
    for(auto& s: sbpi){
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, s.second, bssbo[s.second]);
    }
    _textures = 0;
  }

  // Templated Member Functions

  template<typename T> void uniform(std::string name, const T u);
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]);
  void texture(std::string name, const Texture& texture);

  inline const uint32_t valid()    const { return this->_valid; }    //!< Check Shader Validity
  inline const uint32_t index()    const { return this->_index; }    //!< Retrieve Program Index
  inline const uint32_t textures() const { return this->_textures; } //!< Retrieve Number of Active Textures

  // SSBO and Buffer Binding Methods
  std::unordered_map<GLuint, GLuint> bssbo;
  std::unordered_map<std::string, GLuint> sbpi;
  std::unordered_map<std::string, GLuint> sbpc;
  void ssbo(std::string name);                  //Define an SSBO by name

 // void interface(std::string name);                    //Add SSBO to permitted interface blocks
  void bind(std::string name, Buffer* buf);

private:
  GLint _valid;       //!< Validity of Linked Program
  uint32_t _index;    //!< Index of the Multi-Stage Program
  uint32_t _textures; //!< Number of Bound Textures
};

// Set of Buffer SSBO Indices???
//std::unordered_map<GLuint, GLuint> ShaderProgram::bssbo;

void ShaderProgram::ssbo(std::string name){
  if(sbpi.find(name) != sbpi.end()) return; //Named Binding Point Exists
  sbpi[name] = sbpi.size();
  sbpc[name] = glGetProgramResourceIndex(this->index(), GL_SHADER_STORAGE_BLOCK, name.c_str());
  glShaderStorageBlockBinding(this->index(), sbpc[name], sbpi[name]);  
}

void ShaderProgram::bind(std::string name, Buffer* buf){
  if(sbpi.find(name) == sbpi.end()) return; //Named Binding Point Exists
  bssbo[sbpi[name]] = buf->index();
}

// Multi-Stage Shader Implementations

//! 
struct Shader: ShaderProgram {
private:
  Shader():
  vertexShader(GL_VERTEX_SHADER),
  fragmentShader(GL_FRAGMENT_SHADER),
  geometryShader(GL_GEOMETRY_SHADER){}

public:

  Shader(const std::vector<std::string> shaders, const std::vector<std::string> in):Shader(){

    if(shaders.size() == 2){
      vertexShader.load(shaders[0]);
      this->attach(vertexShader);
      fragmentShader.load(shaders[1]);
      this->attach(fragmentShader);
    }

    else if(shaders.size() == 3){
      vertexShader.load(shaders[0]);
      this->attach(vertexShader);
      geometryShader.load(shaders[1]);
      this->attach(geometryShader);      
      fragmentShader.load(shaders[2]);
      this->attach(fragmentShader);
    }

    else std::cout<<"Number of shaders not recognized."<<std::endl;

    this->attributes(in);
    this->link();
  
  }

  Shader(const std::vector<std::string> shaders, const std::vector<std::string> in, const std::vector<std::string> buf):Shader(shaders, in){
    for(auto& b: buf) ssbo(b);
  }

private:
  ShaderStage vertexShader;
  ShaderStage geometryShader;
  ShaderStage fragmentShader;
};

//! Compute
//!
struct Compute: ShaderProgram {
private:
  Compute():
  computeShader(GL_COMPUTE_SHADER){}

public:

  Compute(const std::string shader):Compute(){
    computeShader.load(shader);
    this->attach(computeShader);
    this->link();
  }

  Compute(const std::string shader, std::vector<std::string> buf):Compute(shader){
    for(auto& b: buf) ssbo(b);
  }

  void dispatch(int x = 1, int y = 1, int z = 1, bool block = true){
    glDispatchCompute(x, y, z);
    if(block) glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  }

private:
  ShaderStage computeShader;
};

// Template Instantiations

template<typename T>
void ShaderProgram::uniform(std::string name, T u){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<typename T, size_t N>
void ShaderProgram::uniform(std::string name, const T (&u)[N]){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<> void ShaderProgram::uniform(std::string name, const bool u){
  glUniform1i(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void ShaderProgram::uniform(std::string name, const int u){
  glUniform1i(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void ShaderProgram::uniform(std::string name, const uint32_t u){
  glUniform1i(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void ShaderProgram::uniform(std::string name, const float u){
  glUniform1f(glGetUniformLocation(this->index(), name.c_str()), u); }

template<> void ShaderProgram::uniform(std::string name, const double u){ //GLSL Intrinsically Single Precision
  glUniform1f(glGetUniformLocation(this->index(), name.c_str()), (float)u); }

template<> void ShaderProgram::uniform(std::string name, const glm::vec2 u){
  glUniform2fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void ShaderProgram::uniform(std::string name, const glm::ivec2 u){
  glUniform2iv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void ShaderProgram::uniform(std::string name, const glm::vec3 u){
  glUniform3fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void ShaderProgram::uniform(std::string name, const float (&u)[3]){
  glUniform3fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void ShaderProgram::uniform(std::string name, const float (&u)[4]){
  glUniform4fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void ShaderProgram::uniform(std::string name, const glm::vec4 u){
  glUniform4fv(glGetUniformLocation(this->index(), name.c_str()), 1, &u[0]); }

template<> void ShaderProgram::uniform(std::string name, const glm::mat3 u){
  glUniformMatrix3fv(glGetUniformLocation(this->index(), name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void ShaderProgram::uniform(std::string name, const glm::mat4 u){
  glUniformMatrix4fv(glGetUniformLocation(this->index(), name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void ShaderProgram::uniform(std::string name, const std::vector<glm::mat4> u){
  glUniformMatrix4fv(glGetUniformLocation(this->index(), name.c_str()), u.size(), GL_FALSE, &u[0][0][0]); }

void ShaderProgram::texture(std::string name, const Texture& texture){
  glActiveTexture(GL_TEXTURE0 + _textures);
  texture.operator()();
  uniform(name, _textures++);
}

/*
//! Deprecate: Move this to generic debug info
static void Compute::limits(){
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
*/

} // end of namespace Tiny

#endif

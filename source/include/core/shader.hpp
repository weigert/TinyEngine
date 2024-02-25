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

  ShaderStage(GLenum shaderType){ this->_index = glCreateShader(shaderType); }
  ~ShaderStage(){ glDeleteShader(this->index()); }

  ShaderStage(GLenum shaderType, const std::string filename):
  ShaderStage(shaderType){
    this->load(filename);
  }

  void load(const std::string filename);  //!< Load and Compile Source File into ShaderStage

  inline const uint32_t valid() const { return this->_valid; }  //!< Check Shader Validity
  inline const uint32_t index() const { return this->_index; }  //!< Retrieve Shader Stage Index

private:
  GLint _valid;    //!< ShaderStage Validity Status
  uint32_t _index; //!< Index of the Shader Stage
};

void ShaderStage::load(const std::string filename){
  int32_t size;
  const std::string result = readGLSLFile(filename, size);
  char* src = const_cast<char*>(result.c_str());
  glShaderSource(this->index(), 1, &src, &size);
  glCompileShader(this->index());
  glGetShaderiv(this->index(), GL_COMPILE_STATUS, &_valid);
  if(!this->valid()) printShaderError(this->index(), filename);
}

//! ShaderProgram is an executable combination of multiple shader stages.
//!
//! An arbitrary set of ShaderStage can be attached to a ShaderProgram.
//! ShaderProgram provides a simple interface for providing named uniforms,
//! textures and storage blocks to the shader.
//! Note that the shader must be active to bind these objects.
//!
struct ShaderProgram {

  ShaderProgram(){ this->_index = glCreateProgram(); }
  ~ShaderProgram(){ glDeleteProgram(this->index()); }

  void attributes(const std::vector<std::string> attributes); //!< Set the Program's Attributes
  void attach(ShaderStage& stage);                            //!< Attach a ShaderStage
  void link();                                                //!< Link all attached ShaderStages
  void use();                                                 //!< Use / Activate the Program

  template<typename T> void uniform(std::string name, const T u);                 //!< Bind a Uniform Value
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]); //!< Bind a Uniform Value (Array)
  template<typename T> void texture(const std::string name, const T& texture);    //!< Bind a Texture
  void storage(const std::string name, const Buffer& buffer);                     //!< Bind a Storage Block Buffer

  inline const uint32_t valid()    const { return this->_valid; }    //!< Check Shader Validity
  inline const uint32_t index()    const { return this->_index; }    //!< Retrieve Program Index
  inline const uint32_t textures() const { return this->_textures; } //!< Retrieve Number of Active Textures
  inline const uint32_t storages() const { return this->_storages; } //!< Retrieve Number of Active Storage Blocks

private:
  GLint _valid;       //!< Validity of Linked Program
  uint32_t _index;    //!< Index of the Multi-Stage Program
  uint32_t _textures; //!< Number of Bound Textures
  uint32_t _storages; //!< Numbre of Bound Storage Blocks
};

void ShaderProgram::attributes(const std::vector<std::string> attrs){
  for(int i = 0; i < attrs.size(); i++)
    glBindAttribLocation(this->index(), i, attrs[i].c_str()); // Activate the Attribute
}

void ShaderProgram::attach(ShaderStage& stage){
  if(stage.valid()) glAttachShader(this->index(), stage.index()); // Attach Shader if Valid (Compiled Succesfully)
}

void ShaderProgram::link(){
  glLinkProgram(this->index()); // Link this Program (all attached ShaderStages)
  glGetProgramiv(this->index(), GL_LINK_STATUS, &_valid); // Get Success State
  if(!this->valid()) printProgramError(this->index());
}

void ShaderProgram::use(){
  glUseProgram(this->index());  // Activate the Program
  _textures = 0;                // Reset Bound Texture State
  _storages = 0;                // Reset Bound Storage Block State
}

// Multi-Stage Shader Implementations

//! Shader is a ShaderProgram that supports a vertex shader,
//! fragment shader and optional geoemtry shader stage.
//!
//! Note that this is just a convenience wrapper class.
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

private:
  ShaderStage vertexShader;
  ShaderStage geometryShader;
  ShaderStage fragmentShader;
};

//! Compute is a ShaderProgram with a single computer shader stage.
//! It provides a convenient dispatch function for executing the
//! compute shader. Note that to provide data to the compute shader,
//! one must bind buffers as storage blocks before dispatch.
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

template<typename T>
void ShaderProgram::texture(const std::string name, const T& texture){
  glActiveTexture(GL_TEXTURE0 + _textures);
  texture.operator()();
  uniform(name, _textures++);
}

void ShaderProgram::storage(const std::string name, const Buffer& buffer){
  uint32_t binding = glGetProgramResourceIndex(this->index(), GL_SHADER_STORAGE_BLOCK, name.c_str());
  glShaderStorageBlockBinding(this->index(), binding, _storages);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _storages, buffer.index());
  _storages++;
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

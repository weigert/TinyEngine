class ShaderBase {
public:

  GLuint program;     //Shader Program ID

  ShaderBase(){
    program = glCreateProgram();        //Generate Shader
  }

  ~ShaderBase(){
    glDeleteProgram(program);
  }

  static std::string readGLSLFile(std::string fileName, int32_t &size);  //Read File
  int addProgram(std::string fileName, GLenum shaderType);        //General Shader Addition
  void compile(GLuint shader);          //Compile and Add File
  void link();                          //Link the entire program
  void use();                           //Use the program
  static void error(GLuint s, bool t);  //Get Compile/Link Error

  static std::unordered_map<std::string, GLuint> sbpi; //Shader Binding Point Index

  static void ssbo(std::string name);                  //Define an SSBO by name
  static void ssbo(std::vector<std::string> names);    //Define a list of SSBOs
  void interface(std::string name);                    //Add SSBO to permitted interface blocks
  void interface(std::vector<std::string> names);      //Add a list of buffers to interface
  template<typename T> static void bind(std::string name, Buffer* buf);

  int boundtextures;                            //Texture Indexing

  template<typename T> void uniform(std::string name, const T u);
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]);
  template<typename T> void texture(std::string name, const T& t);

};

std::unordered_map<std::string, GLuint> ShaderBase::sbpi; //Shader Binding Point Index

std::string ShaderBase::readGLSLFile(std::string file, int32_t &size){
  boost::filesystem::path local_dir = boost::filesystem::path(file).parent_path();

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

int ShaderBase::addProgram(std::string fileName, GLenum shaderType){
  char* src; int32_t size;
  std::string result = readGLSLFile(fileName, size);
  src = const_cast<char*>(result.c_str());

  int shaderID = glCreateShader(shaderType);
  glShaderSource(shaderID, 1, &src, &size);
  compile(shaderID);

  return shaderID;
}

void ShaderBase::compile(GLuint shader){
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(success) glAttachShader(program, shader);
  else        error(shader, true);
}

void ShaderBase::link(){
  glLinkProgram(program);
  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if(!success) error(program, false);
}

void ShaderBase::use(){
  boundtextures = 0;
  glUseProgram(program);
}

void ShaderBase::error(GLuint s, bool t){
  int m;
  if(t) glGetShaderiv(s, GL_INFO_LOG_LENGTH, &m);
  else glGetProgramiv(s, GL_INFO_LOG_LENGTH, &m);
  char* l = new char[m];
  if(t) glGetShaderInfoLog(s, m, &m, l);
  else glGetProgramInfoLog(s, m, &m, l);
  std::cout<<"Linker Error: "<<l<<std::endl;
  delete[] l;
}

void ShaderBase::ssbo(std::string name){
  if(sbpi.find(name) != sbpi.end()) return; //Named Binding Point Exists
  sbpi[name] = sbpi.size();
}

void ShaderBase::ssbo(std::vector<std::string> names){
  for(auto& l: names) ssbo(l);
}

void ShaderBase::interface(std::string name){
  ssbo(name); //Make sure binding point exists
  glShaderStorageBlockBinding(program, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, name.c_str()), sbpi[name]);
}

void ShaderBase::interface(std::vector<std::string> names){
  for(auto& l: names) interface(l);
}

template<typename T>
void ShaderBase::bind(std::string name, Buffer* buf){
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, buf->index);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sbpi[name], buf->index);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<typename T>
void ShaderBase::uniform(std::string name, T u){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<typename T, size_t N>
void ShaderBase::uniform(std::string name, const T (&u)[N]){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<"."<<std::endl; }

template<> void ShaderBase::uniform(std::string name, const bool u){
  glUniform1i(glGetUniformLocation(program, name.c_str()), u); }

template<> void ShaderBase::uniform(std::string name, const int u){
  glUniform1i(glGetUniformLocation(program, name.c_str()), u); }

template<> void ShaderBase::uniform(std::string name, const float u){
  glUniform1f(glGetUniformLocation(program, name.c_str()), u); }

template<> void ShaderBase::uniform(std::string name, const double u){ //GLSL Intrinsically Single Precision
  glUniform1f(glGetUniformLocation(program, name.c_str()), (float)u); }

template<> void ShaderBase::uniform(std::string name, const glm::vec2 u){
  glUniform2fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void ShaderBase::uniform(std::string name, const glm::ivec2 u){
  glUniform2iv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void ShaderBase::uniform(std::string name, const glm::vec3 u){
  glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void ShaderBase::uniform(std::string name, const float (&u)[3]){
  glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void ShaderBase::uniform(std::string name, const float (&u)[4]){
  glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void ShaderBase::uniform(std::string name, const glm::vec4 u){
  glUniform4fv(glGetUniformLocation(program, name.c_str()), 1, &u[0]); }

template<> void ShaderBase::uniform(std::string name, const glm::mat3 u){
  glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void ShaderBase::uniform(std::string name, const glm::mat4 u){
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, &u[0][0]); }

template<> void ShaderBase::uniform(std::string name, const std::vector<glm::mat4> u){
  glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), u.size(), GL_FALSE, &u[0][0][0]); }

template<typename T>
void ShaderBase::texture(std::string name, const T& t){
  glActiveTexture(GL_TEXTURE0 + boundtextures);
  glBindTexture(t.type, t.texture);
  uniform(name, boundtextures++);
}

class Shader : public ShaderBase {
private:

  GLuint vertexShader, geometryShader, fragmentShader;

public:

  template<typename... Args>
  Shader(std::vector<std::string> shaders, std::vector<std::string> in):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    for(int i = 0; i < in.size(); i++)
      glBindAttribLocation(program, i, in[i].c_str());
    link();                             //Link the shader program!
  }

  Shader(std::vector<std::string> shaders):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    link();                             //Link the shader program!
  }

  Shader(std::vector<std::string> shaders, std::vector<std::string> in):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    for(int i = 0; i < in.size(); i++)
      glBindAttribLocation(program, i, in[i].c_str());
    link();                             //Link the shader program!
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

};

class Compute : public ShaderBase {
private:

  GLuint computeShader;

public:

  Compute(std::string shader):ShaderBase(),
  computeShader(addProgram(shader, GL_COMPUTE_SHADER)){
    link();
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

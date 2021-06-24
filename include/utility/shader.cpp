class ShaderBase {
public:

  GLuint program;     //Shader Program ID

  int boundtextures;                            //Texture Indexing
  std::unordered_map<std::string, GLuint> ssbo; //SSBO Storage
  std::unordered_map<std::string, GLuint> sbpi; //Shader Binding Point Index

  ShaderBase(){
    program = glCreateProgram();        //Generate Shader
  }

  ~ShaderBase(){
    glDeleteProgram(program);
  }

  int addProgram(std::string fileName, GLenum shaderType);  //General Shader Addition
  std::string readGLSLFile(std::string fileName, int32_t &size); //Read File
  void compile(GLuint shader);  //Compile and Add File
  void link();                  //Link the entire program
  void error(GLuint s, bool t); //Get Compile/Link Error
  void use();                   //Use the program

  void addBuffer(std::string name);                                 //General Buffer Addition
  template<typename T> void buffer(std::string name, std::vector<T>& buf, bool update = false);
  template<typename T> void retrieve(std::string name, std::vector<T>& buf);
  template<typename T> void texture(std::string name, const T& t);
  template<typename T> void uniform(std::string name, const T u);
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]);

};

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

std::string ShaderBase::readGLSLFile(std::string file, int32_t &size){
  boost::filesystem::path data_dir(boost::filesystem::current_path());
  boost::filesystem::path local_dir = boost::filesystem::path(file).parent_path();

  std::ifstream t;
  std::string fileContent;
  std::string line;

  t.open((data_dir/file).string()); //Read GLSL File Contents
  if(t.is_open()){
    std::stringstream buffer;
    while(!t.eof()){
      getline(t, line);
      if(line.substr(0, 9) == "#include "){
        int includesize = 0;
        buffer << readGLSLFile((local_dir/line.substr(9)).string(), includesize);
      }
      else{
        buffer << line;
        buffer << "\n";
      }
    }
    fileContent = buffer.str();
  }
  else std::cout<<"File opening \""<<file<<"\" failed"<<std::endl;
  t.close();

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

/* Shader Storage Buffer Objects */

void ShaderBase::addBuffer(std::string name){
  unsigned int b;
  glGenBuffers(1, &b);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, b);
  glShaderStorageBlockBinding(program, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, name.c_str()), ssbo.size());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo.size(), b);
  sbpi[name] = ssbo.size();
  ssbo[name] = b;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<typename T>
void ShaderBase::buffer(std::string name, std::vector<T>& buf, bool update){
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[name]);
  if(update) glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, buf.size()*sizeof(T), &buf[0]);
  else glBufferData(GL_SHADER_STORAGE_BUFFER, buf.size()*sizeof(T), &buf[0], GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, sbpi[name], ssbo[name]);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<typename T>
void ShaderBase::retrieve(std::string name, std::vector<T>& buf){
  glGetNamedBufferSubData(ssbo[name], 0, buf.size(), &buf[0]);
}

/* Uniform Setters */

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

/* Rendering Shaders */

class Shader : public ShaderBase {
private:

  GLuint vertexShader, geometryShader, fragmentShader;

public:

  template<typename... Args>
  Shader(slist shaders, slist in):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    for(auto &n : in)                   //Add all Attributes of Shader
      glBindAttribLocation(program, &n - in.begin(), n.c_str());
    link();                             //Link the shader program!
  }

  Shader(slist shaders, slist in):ShaderBase(){
    setup(shaders);                     //Add Individual Shaders
    for(auto &n : in)                   //Add all Attributes of Shader
      glBindAttribLocation(program, &n - in.begin(), n.c_str());
    link();                             //Link the shader program!
  }

  Shader(slist shaders, slist in, slist buf):Shader(shaders, in){
    for(auto&b : buf) addBuffer(b);     //Add Storage Buffers to Shader
  }

  ~Shader(){
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
    glDeleteShader(vertexShader);
  }

  void setup(slist shaders){
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

  Compute(std::string shader):ShaderBase(){        //Generate Shader
    computeShader = addProgram(shader, GL_COMPUTE_SHADER);
    link();                             //Link the shader program!
  }

  Compute(std::string shader, slist buf):Compute(shader){
    for(auto&b : buf) addBuffer(b);     //Add Storage Buffers to Shader
  }

  ~Compute(){
    glDeleteShader(computeShader);
  }

  void dispatch(glm::vec3 workgroups){
     glDispatchCompute(workgroups.x, workgroups.y, workgroups.z);
  }

  static void limits(){
    std::function<int(GLenum)> getInt = [](GLenum E){
      int i; glGetIntegerv(E, &i); return i;
    };
    std::cout<<"Max. SSBO: "<<getInt(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS)<<std::endl;
    std::cout<<"Max. SSBO Block-Size: "<<getInt(GL_MAX_SHADER_STORAGE_BLOCK_SIZE)<<std::endl;
    std::cout<<"Max. Compute Shader Storage Blocks: "<<getInt(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS)<<std::endl;
    std::cout<<"Max. Shared Storage Size: "<<getInt(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE)<<std::endl;

    int m;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &m);
    std::cout<<"Max. Work Groups: "<<m<<std::endl;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &m);
    std::cout<<"Max. Local Size: "<<m<<std::endl;

  }

};

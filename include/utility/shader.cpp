class Shader{
public:
  Shader(std::string vs, std::string fs, slist _list){
    setup(vs, fs);        //Setup the Shader
    for(auto &n : _list)  //Add all Attributes of Shader
      addInput(&n-_list.begin(), n);
    link();
  };

  ~Shader(){
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
  }

  GLuint shaderProgram;   //Shader Program ID
  GLuint vertexShader, fragmentShader;

  void setup(std::string vs, std::string fs);
  void addInput(int pos, std::string attrName);
  void cleanup();
  int  addProgram(std::string fileName, GLenum shaderType);  //General Shader Addition
  std::string readGLSLFile(std::string fileName, int32_t &size); //Read File
  void compile(GLuint shader);  //Compile and Add File
  void link();                  //Link the entire program
  void use();                   //Use the program

  template<typename T> void uniform(std::string name, const T u);
  template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]);
};

void Shader::setup(std::string vs, std::string fs){
  shaderProgram = glCreateProgram();  //Generate Shader

  boost::filesystem::path data_dir(boost::filesystem::current_path());
  vertexShader   = addProgram((data_dir/vs).string(), GL_VERTEX_SHADER);
  fragmentShader = addProgram((data_dir/fs).string(), GL_FRAGMENT_SHADER);
}

int Shader::addProgram(std::string fileName, GLenum shaderType){
  //Read Shader Program from Source
  char* src; int32_t size;
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

  int success, maxLength;  ///Error Handling
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success );
  if(success){
    glAttachShader(shaderProgram, shader);
    return;
  }

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
  char* shaderInfoLog = new char[maxLength];
  glGetShaderInfoLog(shader, maxLength, &maxLength, shaderInfoLog);
  std::cout << "Linker error message : " << shaderInfoLog << std::endl;
  delete shaderInfoLog;
}

void Shader::link(){
  glLinkProgram(shaderProgram);

  int success, maxLength;  //Error Handling
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&success);
  if(success) return; //Yay

  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
  char* shaderProgramInfoLog = new char[maxLength];
  glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);
  std::cout << "Linker error message: " << shaderProgramInfoLog << std::endl;
  delete shaderProgramInfoLog;
}

void Shader::use(){
  glUseProgram(shaderProgram);
}

std::string Shader::readGLSLFile(std::string file, int32_t &size){
  std::ifstream t;
  std::string fileContent;

  t.open(file);     //Read GLSL File Contents
  if(t.is_open()){
    std::stringstream buffer;
    buffer << t.rdbuf();
    fileContent = buffer.str();
  }
  else std::cout<<"File opening failed"<<std::endl;
  t.close();

  size = fileContent.length();  //Set the Size
  return fileContent;
}

/* Uniform Setters */

template<typename T>
void Shader::uniform(std::string name, T u){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<std::endl;
};

template<typename T, size_t N> void uniform(std::string name, const T (&u)[N]){
  std::cout<<"Error: Data type not recognized for uniform "<<name<<std::endl;
}

template<> void Shader::uniform(std::string name, const bool u){
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), u);
}

template<> void Shader::uniform(std::string name, const int u){
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), u);
}

template<> void Shader::uniform(std::string name, const float u){
  glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), u);
}

template<> void Shader::uniform(std::string name, const double u){ //GLSL Intrinsically Single Precision
  glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), (float)u);
}

template<> void Shader::uniform(std::string name, const glm::vec2 u){
  glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &u[0]);
}

template<> void Shader::uniform(std::string name, const glm::vec3 u){
  glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &u[0]);
}

template<> void Shader::uniform(std::string name, const glm::vec4 u){
  glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &u[0]);
}

template<> void Shader::uniform(std::string name, const float (&u)[3]){
  glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &u[0]);
}

template<> void Shader::uniform(std::string name, const float (&u)[4]){
  glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &u[0]);
}

template<> void Shader::uniform(std::string name, const glm::mat3 u){
  glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &u[0][0]);
}

template<> void Shader::uniform(std::string name, const glm::mat4 u){
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &u[0][0]);
}

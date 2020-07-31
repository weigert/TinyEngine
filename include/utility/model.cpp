struct Primitive{

  Primitive(){
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    addBuffers(2);
  }

  ~Primitive(){
    glDisableVertexAttribArray(vao);
    clearBuffers();
    glDeleteVertexArrays(1, &vao);
  }

  void addBuffers(int n){
    GLuint nvbo;
    for(int i = 0; i < n; i++){
      glGenBuffers(1, &nvbo);
      vbo.push_back(nvbo);
    }
  }

  void clearBuffers(){
    while(!vbo.empty()){
      glDeleteBuffers(1, &(vbo.back()));
      vbo.pop_back();
    }
  }

  GLuint vao;
  std::vector<GLuint> vbo;
  size_t SIZE = 4;

  template<typename T>
  void bind(int index, int count, int size, T* data){
    glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(T), data, GL_DYNAMIC_DRAW);
    attrib<T>(index, size);
  }

  template<typename T>
  void attrib(int index, int size){
    std::cout<<"Data type not recognized."<<std::endl;
  }

  void render(GLenum mode = GL_TRIANGLE_STRIP){
    glBindVertexArray(vao);
    glDrawArrays(mode, 0, SIZE);
  }

  glm::mat4 model = glm::mat4(1.0);
  glm::vec3 pos   = glm::vec3(0.0);
  glm::vec3 scale = glm::vec3(1.0);
  float rot = 0.0;
  const glm::vec3 up = glm::vec3(0, 1, 0);

  void move(glm::vec3 p, float r = 0, glm::vec3 s = glm::vec3(1)){
    pos = p; rot = r; scale = s;      //Change Values
    model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0), pos), glm::radians(rot), up), scale);
  }

  void shift(glm::vec3 p, float r = 0, glm::vec3 s = glm::vec3(0)){
    pos += p; rot += r; scale += s;   //Change Values
    model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0), pos), glm::radians(rot), up), scale);
  }
};

template<>
void Primitive::attrib<GLfloat>(int index, int size){
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
}

//Primitive Shapes (Pre-Made)

struct Square2D: Primitive{
  GLfloat vert[8] = {-1.0,  1.0, -1.0, -1.0,  1.0,  1.0,  1.0, -1.0};
  GLfloat tex [8] = { 0.0,  0.0,  0.0,  1.0,  1.0,  0.0,  1.0,  1.0};

  Square2D():Primitive(){
    bind(0, 8, 2, &vert[0]);
    bind(1, 8, 2, &tex[0]);
  }
};

struct Square3D: Primitive{
  GLfloat vert[12] =  {-1.0,  1.0,  0.0, -1.0, -1.0,  0.0,  1.0,  1.0,  0.0,  1.0, -1.0,  0.0};
  GLfloat tex [8]   = { 0.0,  0.0,  0.0,  1.0,  1.0,  0.0,  1.0,  1.0};

  Square3D():Primitive(){
    bind(0, 12, 3, &vert[0]);
    bind(1, 8,  2, &tex[0]);
  }
};

class Model: public Primitive {
public:
  Model():Primitive(){
    glGenBuffers(1, &ibo);  //Additional Index Buffer
    addBuffers(1);          //Additional Color Buffer
  }

  Model(std::function<void(Model*)> c):Model(){
    construct(c);
  }

  template<typename... T>   //Pass Parameters to Model Constructing Function
  Model(std::function<void(Model*, T...)> c, T... t):Model(){
    construct(c, t...);
  }

  ~Model(){
    glDisableVertexAttribArray(vao);
    clearBuffers();
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
  }

  std::vector<GLfloat>  positions, normals, colors;
  std::vector<GLuint>   indices;

  bool indexed = true;
	GLuint ibo;

  void update(){
    glBindVertexArray(vao);
    bind(0, positions.size(), 3, &positions[0]);
    if(!normals.empty()) bind(1, normals.size(), 3, &normals[0]);
    if(!colors.empty()) bind(2, colors.size(), 4, &colors[0]);

    SIZE = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIZE*sizeof(GLuint), &indices[0], GL_DYNAMIC_DRAW);
  }

  void construct(std::function<void(Model*)> constructor){
    positions.clear();
    normals.clear();
    colors.clear();
    indices.clear();

    (constructor)(this);  //Call user-defined constructor
    update();                //Update VAO / VBO / IBO
  }

  template<typename... T>
  void construct(std::function<void(Model*, T...)> constructor, T... t){
    positions.clear();
    normals.clear();
    colors.clear();
    indices.clear();

    (constructor)(this, t...);  //Call user-defined constructor
    update();                   //Update VAO / VBO / IBO
  }

  void render(GLenum mode = GL_TRIANGLES){
    glBindVertexArray(vao);
    if(indexed){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glDrawElements(mode, SIZE, GL_UNSIGNED_INT, 0);
    }
    else glDrawArrays(mode, 0, positions.size()/3);
  }
};

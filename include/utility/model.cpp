struct Model {

  GLuint vao;                                       //Vertex Array Object
  size_t SIZE = 0;                                  //Number of Vertices

  std::unordered_map<std::string, int> bindings;        //Binding Points of Attributes
  std::unordered_map<std::string, Buffer*> buffers;     //Owned Buffer

  Model(){
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  }

  ~Model(){
    glDisableVertexAttribArray(vao);
    glDeleteVertexArrays(1, &vao);
    for(auto& buf: buffers)    //Delete Owned Buffers
      delete buf.second;
  }

  Model(std::vector<std::string> nbinding):Model(){     //Generate Model with Defined Bindings
    for(auto& b: nbinding){
      bindings[b] = bindings.size();          //Give the Named Binding Point an Index
      glEnableVertexAttribArray(bindings[b]); //Enable the Binding Point
    }
  }

  template<typename T>
  void bind(std::string binding, Buffer* buf, bool owned = false){  //Bind a specific buffer to a binding point
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buf->index);
    glBindVertexBuffer(bindings[binding], buf->index, 0, sizeof(T));
    glVertexAttribFormat(bindings[binding], sizeof(T)/sizeof(GL_FLOAT), GL_FLOAT, GL_FALSE, 0);
    if(owned) buffers[binding] = buf;
  }

  GLuint iindex;
  bool indexed = false;
  void index(Buffer* buf, bool owned = false){
    indexed = true;
    SIZE = buf->SIZE;
    iindex = buf->index;
    if(owned) buffers["index"] = buf;
  }

  glm::mat4 model = glm::mat4(1.0f);                //Model Matrix

  void render(GLenum mode = GL_TRIANGLE_STRIP){
    glBindVertexArray(vao);
    if(indexed){
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iindex);
      glDrawElements(mode, SIZE, GL_UNSIGNED_INT, 0);
    }
    else glDrawArrays(mode, 0, SIZE);
  }

};

struct Point: Model {
  Buffer vert;
  Point():Model({"vert"}),
  vert({0.0f, 0.0f, 0.0f}){
    bind<glm::vec3>("vert", &vert);
    SIZE = 1;
  }
};

struct Square2D : Model {
  Buffer vert, tex;                                  //2 Buffers
  Square2D():Model({"in_Quad", "in_Tex"}),                  //2 Binding Points
  vert({-1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  1.0f}),   //Initialize Buffer "vert"
  tex({ 0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f}){    //Initialize Buffer "tex"
    bind<glm::vec2>("in_Quad", &vert);                      //Bind vert as glm::vec2
    bind<glm::vec2>("in_Tex", &tex);                        //Bind tex as glm::vec2
    SIZE = 4;                                               //Set total vertex number to 4
  }
};

struct Square3D : Model {
  Buffer vert, tex;
  Square3D():Model({"in_Quad", "in_Tex"}),
  vert({-1.0f,  1.0f,  0.0f, -1.0f, -1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f, -1.0f,  0.0f}),
  tex({ 0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f}){
    bind<glm::vec3>("in_Quad", &vert);
    bind<glm::vec2>("in_Tex", &tex);
    SIZE = 4;
  }
};

struct Cube : Model {
  Buffer vert, tex;
  Cube():Model({"in_Quad", "in_Tex"}),
  vert({ /* Front */ 1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,
         /* Back  */-1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
         /* Left  */-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
         /* Right */ 1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,
         /* Top   */-1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
         /* Bottom*/ 1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f }),
  tex({ 0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f}){
    bind<glm::vec3>("in_Quad", &vert);
    bind<glm::vec2>("in_Tex", &tex);
    SIZE = 36;
  }
};

struct Gizmo: Model {
  Buffer vert, tex;
  Gizmo():Model({"in_Quad", "in_Tex"}),
  vert({0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}),
  tex({ 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}){
    bind<glm::vec3>("in_Quad", &vert);
    bind<glm::vec3>("in_Tex", &tex);
    SIZE = 6;
  }
};

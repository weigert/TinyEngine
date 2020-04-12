class Model{
public:
  Model(){setup();};

  Model(std::function<void(Model* m)> c){
    setup();
    construct(c);
  };

  ~Model(){
    glDisableVertexAttribArray(vao);
    glDeleteBuffers(3, vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
  }

  std::vector<GLfloat>  positions;
  std::vector<GLfloat>  normals;
  std::vector<GLfloat>  colors;
  std::vector<GLuint>   indices;

	GLuint vbo[3], vao, ibo;

  glm::mat4 model = glm::mat4(1.0f);  //Model Matrix
  glm::vec3 pos = glm::vec3(0.0f);    //Model Position

  void setup();
  void update();
  void construct(std::function<void(Model* m)> constructor){
    positions.clear();  //Clear all Data
    normals.clear();
    colors.clear();
    indices.clear();

    (constructor)(this);  //Call user-defined constructor
    update();             //Update VAO / VBO / IBO
  };

  void translate(const glm::vec3 &axis);
  void rotate(const glm::vec3 &axis, float angle);

  void render(GLenum T);
};

void Model::setup(){
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(3, vbo);
  glGenBuffers(1, &ibo);
}

void Model::update(){
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);      //Positions
  glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);      //Normals
  glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);      //Colors
  glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); //Indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Model::translate(const glm::vec3 &axis){
  model = glm::translate(model, axis);
  pos += axis;
}

void Model::rotate(const glm::vec3 &axis, float angle){
  model = glm::translate(glm::rotate(glm::translate(model, -pos), angle, axis), pos);
}

void Model::render(GLenum T){
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(T, indices.size(), GL_UNSIGNED_INT, 0);
}

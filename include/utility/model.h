#pragma once
class Model{
public:
  //Constructor
  Model(){
    setup();
  };


  //Model Information
  std::vector<GLfloat> positions;
  std::vector<GLfloat> normals;
  std::vector<GLuint> indices;

  // Create variables for storing the ID of our VAO and VBO
	GLuint vbo[2], vao, ibo;

  //Model Matrix (helps translate vertices from model space to real space)
  glm::mat4 model = glm::mat4(1.0f);
  glm::vec3 pos = glm::vec3(0.0f);  //Chunkspace Position!
  glm::vec3 cpos = glm::vec3(0);

  //Read File Functions
  void setup();
  void update();
  void cleanup();

  //Model Generation Functions
  //void fromWorld(World &world);

  //Position Handling Functions
  void translate(const glm::vec3 &axis);
  void rotate(const glm::vec3 &axis, float angle);

  //Render Function to Render Stuff with the shader, so we can do this to each model individually
  void render();
};

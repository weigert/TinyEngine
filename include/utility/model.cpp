#include "model.h"
//Dependencies

void Model::setup(){
  //Setup VAO and VBOs
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(2, vbo);
  glGenBuffers(1, &ibo);

  update();
}

void Model::update(){
  glBindVertexArray(vao);
  //Positions Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(GLfloat), &positions[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //Normal Buffer
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //Index Buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
}

void Model::cleanup(){
  glDisableVertexAttribArray(vao);
  glDeleteBuffers(2, vbo);
  glDeleteBuffers(1, &ibo);
  glDeleteVertexArrays(1, &vao);
}

void Model::translate(const glm::vec3 &axis){
  //Construct the Translation Matrix
  pos += axis;
  model = glm::translate(model, axis);
}

void Model::rotate(const glm::vec3 &axis, float angle){
  //Add the rotation operation to the model matrix
  model = glm::translate(glm::rotate(glm::translate(model, -pos), angle, axis), pos);
}

void Model::render(){
  glBindVertexArray(vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

/*
void Model::fromWorld(World &w){

  //Clear the Containers
  indices.clear();
  positions.clear();
  normals.clear();

  //Loop over all positions and add the triangles!
  for(int i = 0; i < w.dim.x-1; i++){
    for(int j = 0; j < w.dim.y-1; j++){

      //Add to Position Vector
      glm::vec3 a = glm::vec3(i, w.scale*w.heightmap[i][j], j);
      glm::vec3 b = glm::vec3(i+1, w.scale*w.heightmap[i+1][j], j);
      glm::vec3 c = glm::vec3(i, w.scale*w.heightmap[i][j+1], j+1);
      glm::vec3 d = glm::vec3(i+1, w.scale*w.heightmap[i+1][j+1], j+1);

      //UPPER TRIANGLE

      //Add Indices
      indices.push_back(positions.size()/3+0);
      indices.push_back(positions.size()/3+1);
      indices.push_back(positions.size()/3+2);

      positions.push_back(a.x);
      positions.push_back(a.y);
      positions.push_back(a.z);
      positions.push_back(b.x);
      positions.push_back(b.y);
      positions.push_back(b.z);
      positions.push_back(c.x);
      positions.push_back(c.y);
      positions.push_back(c.z);

      glm::vec3 n1 = glm::cross(a-b, c-b);

      for(int i = 0; i < 3; i++){
        normals.push_back(n1.x);
        normals.push_back(n1.y);
        normals.push_back(n1.z);
      }

      //Lower Triangle

      indices.push_back(positions.size()/3+0);
      indices.push_back(positions.size()/3+1);
      indices.push_back(positions.size()/3+2);

      positions.push_back(d.x);
      positions.push_back(d.y);
      positions.push_back(d.z);
      positions.push_back(c.x);
      positions.push_back(c.y);
      positions.push_back(c.z);
      positions.push_back(b.x);
      positions.push_back(b.y);
      positions.push_back(b.z);

      glm::vec3 n2 = glm::cross(d-c, b-c);

      for(int i = 0; i < 3; i++){
        normals.push_back(n2.x);
        normals.push_back(n2.y);
        normals.push_back(n2.z);
      }
    }
  }
}
*/

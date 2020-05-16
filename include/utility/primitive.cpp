enum TinyPrimitive {
  CUBE,
  SQUARE2D,
  SQUARE3D
};

template<TinyPrimitive P>
struct PData{
  const GLfloat* vert, tex;
  const int vertsize, texsize;
  const int vertcount, texcount;
};

template<TinyPrimitive P>
struct Primitive{
  PData<P> raw;

  Primitive(){
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(2, &vbo[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, raw.vertcount*sizeof(GLfloat), &raw.vert[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, raw.vertsize, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, raw.texcount*sizeof(GLfloat), &raw.tex[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, raw.texsize, GL_FLOAT, GL_FALSE, 0, 0);
  }

  ~Primitive(){
    glDisableVertexAttribArray(vao);
    glDeleteBuffers(2, vbo);
    glDeleteVertexArrays(1, &vao);
  }

  void render(){
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  GLuint vao, vbo[2];
  glm::mat4 model = glm::mat4(1.0);
};

//Vertex Data as Necessary

template<> struct PData<SQUARE2D>{
  const GLfloat vert[8] = {-1.0, -1.0, -1.0,  1.0,  1.0, -1.0,  1.0,  1.0};
  const GLfloat tex [8] = { 0.0,  1.0,  0.0,  0.0,  1.0,  1.0,  1.0,  0.0};
  const int vertcount = 8;
  const int texcount = 8;
  const int vertsize = 2;
  const int texsize = 2;
};

template<> struct PData<SQUARE3D>{
  const GLfloat vert[12] =  {-1.0, -1.0,  0.0, -1.0,  1.0,  0.0,  1.0, -1.0,  0.0,  1.0,  1.0,  0.0};
  const GLfloat tex [8]  =  { 0.0,  1.0,  0.0,  0.0,  1.0,  1.0,  1.0,  0.0};
  const int vertcount = 12;
  const int texcount = 8;
  const int vertsize = 3;
  const int texsize = 2;
};

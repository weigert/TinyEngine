#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <random>

const size_t WIDTH = 1200;
const size_t HEIGHT = 800;

int main( int argc, char* args[] ) {

  // Setup Window, Camera

  Tiny::window("Shader SSAO", 1200, 800);

  Tiny::cam::ortho ortho(Tiny::view.WIDTH, Tiny::view.HEIGHT, -50.0f, 50.0f, 50.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0, 0, 0));
  ortho.update();
  orbit.update();

  Tiny::cam::camera cam(ortho, orbit);
  Tiny::event.handler = cam.handler;
  
  // Setup gBuffer

  Tiny::GBuffer gBuffer(WIDTH, HEIGHT);

  // Setup SSAO Texture
  std::uniform_real_distribution<GLfloat> randomFloats(-1.0, 1.0); // generates random floats between 0.0 and 1.0
  std::default_random_engine generator;

  Tiny::Target ssaofbo(WIDTH, HEIGHT);

  Tiny::Texture ssaotex(WIDTH, HEIGHT, {GL_R16F, GL_RED, GL_FLOAT});

  ssaofbo.bind(ssaotex, GL_COLOR_ATTACHMENT0);

  // generate sample kernel
  // ----------------------
  std::vector<glm::vec3> ssaoKernel;
  for (unsigned int i = 0; i < 64; ++i){
      glm::vec3 sample(randomFloats(generator), randomFloats(generator), 0.5f*randomFloats(generator)+0.5f);
      sample = glm::normalize(sample);
      sample *= (0.5f*randomFloats(generator)+0.5f);
      float scale = float(i) / 64.0f;
      scale = 0.1f + scale*scale*(1.0f-0.1f);
      sample *= scale;
      ssaoKernel.push_back(sample);
  }

  // generate noise texture
  // ----------------------
  std::vector<glm::vec3> ssaoNoise;
  for (unsigned int i = 0; i < 16; i++) {
      glm::vec3 noise(randomFloats(generator), randomFloats(generator), 0.0f); // rotate around z-axis (in tangent space)
      ssaoNoise.push_back(noise);
  }

  Tiny::Texture noisetex(4, 4, {GL_RGBA32F, GL_RGBA, GL_FLOAT}, &ssaoNoise[0]);
  glBindTexture(GL_TEXTURE_2D, noisetex.index());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  Tiny::Square2D flat;

  // Setup Model, Instance

  Tiny::Model conemodel({"in_Position", "in_Normal"});
  std::vector<glm::vec4> conepos;
  std::vector<glm::vec3> conenormal;

  for(int i = 0; i < 32; i++){

    float phiA = 2.0f*3.14159265f*(float)i/15.0f;
    float phiB = 2.0f*3.14159265f*(float)(i+1)/15.0f;

    conepos.push_back(glm::vec4(sin(phiA), -1, cos(phiA), 1));
    conepos.push_back(glm::vec4(sin(phiB), -1, cos(phiB), 1));
    conepos.push_back(glm::vec4(0, 1, 0, 1));

    conenormal.push_back(glm::vec3(sin(phiA), 0.25, cos(phiA)));
    conenormal.push_back(glm::vec3(sin(phiB), 0.25, cos(phiB)));
    conenormal.push_back(glm::vec3(sin(0.5f*(phiA + phiB)), 0.25, cos(0.5f*(phiA + phiB))));
  }

  Tiny::Buffer coneposbuf(conepos);
  Tiny::Buffer conenormalbuf(conenormal);
  conemodel.bind<glm::vec4>("in_Position", coneposbuf);
  conemodel.bind<glm::vec3>("in_Normal", conenormalbuf);

  Tiny::Instance treeparticle(conemodel);  //Particle system based on this model
  Tiny::Buffer modelbuf;
  treeparticle.bind<glm::mat4>(modelbuf);      //Update treeparticle system
  std::vector<glm::mat4> treemodels;
  for(int x = 0; x <= 32; x++)
  for(int y = 0; y <= 32; y++){
    glm::vec3 center = glm::vec3(x - 16, 0, y - 16);
    glm::vec3 offset = 0.5f*glm::vec3(randomFloats(generator), 0.0, randomFloats(generator));
    glm::mat4 translate = glm::translate(glm::mat4(1), center+offset);
    treemodels.push_back(translate);
  }
  modelbuf.set(treemodels);

  // Setup Shaders

  Tiny::Shader modelshader({"shader/model.vs", "shader/model.fs"}, {"in_Position", "in_Normal", "in_Model"});
  Tiny::Shader ssaoshader({"shader/ssao.vs", "shader/ssao.fs"}, {"in_Quad", "in_Tex"});
  Tiny::Shader imageshader({"shader/image.vs", "shader/image.fs"}, {"in_Quad", "in_Tex"});

  float ssaoradius = 1.5f;

  Tiny::view.pipeline = [&](){

    gBuffer.clear(glm::vec4(0,0,0,1));
    gBuffer.target();
    modelshader.use();
    modelshader.uniform("proj", cam.projection.proj());
    modelshader.uniform("view", cam.control.view());
    modelshader.uniform("color", glm::vec3(1.0f));
    treeparticle.render(GL_TRIANGLES, treemodels.size());

    // SSAO Texture

    ssaofbo.target();
    ssaofbo.clear(glm::vec4(0));
    ssaoshader.use();
    for (unsigned int i = 0; i < 64; ++i)
      ssaoshader.uniform("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    ssaoshader.uniform("projection", cam.projection.proj());
    ssaoshader.texture("gPosition", gBuffer.position());
    ssaoshader.texture("gNormal", gBuffer.normal());
    ssaoshader.texture("texNoise", noisetex);
    ssaoshader.uniform("radius", ssaoradius);
    flat.render();

    //Render Scene to Screen

    Tiny::view.target(glm::vec3(0));    //Prepare Target
    imageshader.use();
    imageshader.texture("gPosition", gBuffer.position());
    imageshader.texture("gNormal", gBuffer.normal());
    imageshader.texture("gColor", gBuffer.color());
    imageshader.texture("gDepth", gBuffer.depth());
    imageshader.texture("ssaoTex", ssaotex);
    imageshader.uniform("view", cam.control.view());
    flat.render();

  };

  Tiny::loop([](){
  });

  Tiny::quit();

  return 0;

}

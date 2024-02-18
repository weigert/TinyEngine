#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>
#include <TinyEngine/image>

#define SPARTICLES 64
#define NPARTICLES (SPARTICLES*SPARTICLES)

int main( int argc, char* args[] ) {

  Tiny::view.vsync = false;
  Tiny::window("N-Body Gravity", 1200, 800);

  Tiny::cam::orthogonal ortho({Tiny::view.WIDTH, Tiny::view.HEIGHT}, {-200.0f, 200.0f}, 500.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(0));
  Tiny::camera cam(ortho, orbit);
  cam.hook();

  bool paused = true;
  Tiny::event.press[SDLK_p]([&paused](bool pressed){
    if(!pressed) paused = !paused;
  });

  //Get Positions and Velocities
  std::vector<glm::vec4> position;
  std::vector<glm::vec4> velocity;
  std::vector<float> mass;
  for(int i = 0; i < NPARTICLES; i++){
    position.push_back(((glm::vec4(rand()%10000, rand()%10000, rand()%10000, 10000.0f)-5000.0f)/5000.0f));
    velocity.emplace_back(0.0, 0.0, 0.0, 0.0);
    mass.push_back(1.0f);
  }

  //3 Buffers
  Tiny::Buffer posbuf(position);
  Tiny::Buffer velbuf(velocity);
  Tiny::Buffer massbuf(mass);

  //Compute Shader with SSBO Binding Points
  Tiny::Compute compute_v("shader/gravity_v.cs");
  Tiny::Compute compute_p("shader/gravity_p.cs");

  //Use the Buffer as an Attribute of a Model VAO
  Tiny::Model particles({"in_Pos"});
  particles.bind<glm::vec4>("in_Pos", posbuf);

  //Visualization Shader, does not need attributes
  Tiny::Shader particleShader({"shader/particle.vs", "shader/particle.fs"}, {"in_Pos"});

  //Define the rendering pipeline
  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec3(0));	//Clear Screen to white
    particleShader.use();
    particleShader.uniform("vp", cam.vp());
    particles.render(GL_POINTS, NPARTICLES);

  };

  //Execute the render loop
  Tiny::loop([&](){

    if(paused) return;

    compute_v.use();
    compute_v.uniform("size", NPARTICLES);
    compute_v.storage("position", posbuf);
    compute_v.storage("velocity", velbuf);
    compute_v.storage("mass", massbuf);
    compute_v.dispatch(2, 2);

    compute_p.use();
    compute_p.uniform("size", NPARTICLES);
    compute_p.storage("position", posbuf);
    compute_p.storage("velocity", velbuf);
    compute_p.storage("mass", massbuf);
    compute_p.dispatch(2, 2);

  });

  Tiny::quit();

  return 0;

}
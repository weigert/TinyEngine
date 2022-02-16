#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>

// General Parameters

const int NX = 400;
const int NY = 200;
const int Q = 9;

int main(int argc, char* argv[]){

  Tiny::view.vsync = false;
  Tiny::view.antialias = 0;
  Tiny::window("Lattice Boltzmann Method", 1200, 600);

  // Initialize our Arrays

  Buffer f(NX*NY*Q, (float*)NULL);      //Raw F Buffer
  Buffer fprop(NX*NY*Q, (float*)NULL);  //Raw FProp Buffer
  Buffer b(NX*NY, (float*)NULL);        //Boundary Condition

  // Computed Quantities (For Efficiency)

  Buffer rho(NX*NY, (float*)NULL);
  Buffer v(NX*NY, (glm::vec2*)NULL);

  // Initialization Shader

  Compute init("shader/init.cs", {"f", "fprop", "b"});
  init.bind<float>("f", &f);
  init.bind<float>("fprop", &fprop);
  init.bind<float>("b", &b);

  // Collision and Streaming Compute Shaders

  Compute collide("shader/collide.cs", {"f", "fprop", "b", "rho", "v"});
  collide.bind<float>("f", &f);
  collide.bind<float>("fprop", &fprop);
  collide.bind<float>("b", &b);
  collide.bind<float>("rho", &rho);
  collide.bind<glm::vec2>("v", &v);

  Compute stream("shader/stream.cs", {"f", "fprop", "b"});
  stream.bind<float>("f", &f);
  stream.bind<float>("fprop", &fprop);
  stream.bind<float>("b", &b);

  // Visualization Shader (Flat2D, Samples SSBO)

  Shader view({"shader/view.vs", "shader/view.fs"}, {"in_Quad", "in_Tex"}, {"f", "fprop", "b", "rho", "v"});
  view.bind<float>("f", &f);
  view.bind<float>("fprop", &fprop);
  view.bind<float>("b", &b);
  view.bind<float>("rho", &rho);
  view.bind<glm::vec2>("v", &v);
  Square2D flat;

  // Initialize!

  init.use();
  init.uniform("NX", NX);
  init.uniform("NY", NY);
  init.dispatch(NX/25, NY/25);

  // Loop

  int n = 0;

  Tiny::view.interface = [](){};
  Tiny::event.handler = [](){};
  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec4(0,0,0,1));
    view.use(); 																		//Setup Shader
    view.uniform("NX", NX);
    view.uniform("NY", NY);
    flat.render();

  };

  Tiny::loop([&](){

    collide.use();
    collide.uniform("NX", NX);
    collide.uniform("NY", NY);
    collide.dispatch(NX/25, NY/25);

    stream.use();
    stream.uniform("NX", NX);
    stream.uniform("NY", NY);
    stream.dispatch(NX/25, NY/25);

  });

  Tiny::quit();

  return 0;

}

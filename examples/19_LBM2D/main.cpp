#include <TinyEngine/TinyEngine>

// General Parameters

const int NX = 256;
const int NY = 128;
const int Q = 9;

int main(int argc, char* argv[]){

  Tiny::view.vsync = false;
  Tiny::view.antialias = 0;
  Tiny::benchmark = true;
  Tiny::window("Lattice Boltzmann Method 2D", 800, 400);

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

  // Initialize the Boundary Condition (if you like!)
  float* boundary = new float[NX*NY];
  for(size_t x = 0; x < NX; x++){
    for(size_t y = 0; y < NY; y++){

      boundary[x*NY + y] = 0.0f;

      if(x < 2*NX/8 && y < NY/2 && x >= 1*NX/8){
        boundary[x*NY + y] = 1.0f;
      }

      if(x < 4*NX/8 && y > NY/2 && x >= 3*NX/8){
        boundary[x*NY + y] = 1.0f;
      }

      if(x < 6*NX/8 && y < NY/2 && x >= 5*NX/8){
        boundary[x*NY + y] = 1.0f;
      }

      if(x < 8*NX/8 && y > NY/2 && x >= 7*NX/8){
        boundary[x*NY + y] = 1.0f;
      }
      
      /*

    //  if(glm::length(glm::vec2(x,y) - glm::vec2(NX/4, NY/2)) < 10)
    //    boundary[x*NY + y] = 1.0f;

      if(glm::length(glm::vec2(x,y) - glm::vec2(NX/2, NY/4)) < 10)
        boundary[x*NY + y] = 1.0f;

      if(glm::length(glm::vec2(x,y) - glm::vec2(NX/2, 3*NY/4)) < 10)
        boundary[x*NY + y] = 1.0f;

      if(glm::length(glm::vec2(x,y) - glm::vec2(3*NX/4, NY/2)) < 10)
        boundary[x*NY + y] = 1.0f;
        */

    }
  }
  b.fill(NX*NY, boundary);

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
  init.dispatch(NX/32, NY/32);

  // Loop

  int n = 0;
  bool paused = true;

  Tiny::view.interface = [](){};
  Tiny::event.handler = [&](){
    if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
      paused = !paused;
  };
  Tiny::view.pipeline = [&](){

    Tiny::view.target(glm::vec4(0,0,0,1));
    view.use(); 																		//Setup Shader
    view.uniform("NX", NX);
    view.uniform("NY", NY);
    flat.render();

  };

  Tiny::loop([&](){

    if(paused) return;

    collide.use();
    collide.uniform("NX", NX);
    collide.uniform("NY", NY);
    collide.dispatch(NX/32, NY/32);

    stream.use();
    stream.uniform("NX", NX);
    stream.uniform("NY", NY);
    stream.dispatch(NX/32, NY/32);

  });

  Tiny::quit();
  delete[] boundary;

  return 0;

}

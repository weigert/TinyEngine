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

  Tiny::Buffer<float> f(NX*NY*Q, (float*)NULL);      //Raw F Buffer
  Tiny::Buffer<float> fprop(NX*NY*Q, (float*)NULL);  //Raw FProp Buffer
  Tiny::Buffer<float> b(NX*NY, (float*)NULL);        //Boundary Condition

  // Computed Quantities (For Efficiency)

  Tiny::Buffer<float> rho(NX*NY, (float*)NULL);
  Tiny::Buffer<glm::vec2> v(NX*NY, (glm::vec2*)NULL);

  // Initialization Shader

  Tiny::Compute init("shader/init.cs", {"f", "fprop", "b"});
  init.bind("f", &f);
  init.bind("fprop", &fprop);
  init.bind("b", &b);

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

  Tiny::Compute collide("shader/collide.cs", {"f", "fprop", "b", "rho", "v"});
  collide.bind("f", &f);
  collide.bind("fprop", &fprop);
  collide.bind("b", &b);
  collide.bind("rho", &rho);
  collide.bind("v", &v);

  Tiny::Compute stream("shader/stream.cs", {"f", "fprop", "b"});
  stream.bind("f", &f);
  stream.bind("fprop", &fprop);
  stream.bind("b", &b);

  // Visualization Shader (Flat2D, Samples SSBO)

  Tiny::Shader view({"shader/view.vs", "shader/view.fs"}, {"in_Quad", "in_Tex"}, {"f", "fprop", "b", "rho", "v"});
  view.bind("f", &f);
  view.bind("fprop", &fprop);
  view.bind("b", &b);
  view.bind("rho", &rho);
  view.bind("v", &v);
  Tiny::Square2D flat;

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

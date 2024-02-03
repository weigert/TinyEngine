#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

const int RES = 64;

int main( int argc, char* args[] ) {


	//Initialize a Window
  Tiny::view.vsync = false;
  Tiny::view.antialias = 0;

  Tiny::view.pointSize = 2.0f;

	Tiny::window("GPU Cloth Simulation", 800, 800);

  glDisable(GL_CULL_FACE);

  Tiny::cam::ortho ortho(Tiny::view.WIDTH, Tiny::view.HEIGHT, -100.0f, 100.0f, 20.0f);
  Tiny::cam::orbit orbit(glm::vec3(1, 0, 0), glm::vec3(RES/2, RES/2, 0));
  ortho.update();
  orbit.update();

  Tiny::cam::camera cam(ortho, orbit);

  bool paused = true;
	Tiny::view.interface = [&](){};
  Tiny::event.handler = [&](){

		cam.handler();

		if(!Tiny::event.press.empty() && Tiny::event.press.back() == SDLK_p)
			paused = !paused;

	};

  // Create an Array of Points as an SSBO, Render as Point Model

  std::vector<glm::vec4> vertices(4*RES*RES, glm::vec4(0));

  for(size_t i = 0; i < RES; i++)
  for(size_t j = 0; j < RES; j++){
    vertices[0*RES*RES + i*RES + j] = glm::vec4(i,j, (float)(rand()%101)/100.0f - 0.5,1);
    vertices[1*RES*RES + i*RES + j] = vertices[i*RES + j] ;
    vertices[2*RES*RES + i*RES + j] = vertices[i*RES + j] ;
    vertices[3*RES*RES + i*RES + j] = vertices[i*RES + j] ;
  }

  Tiny::Buffer verticebuf(vertices);

  // Raw Point Model

  Tiny::Shader pointShader({"shader/point.vs", "shader/point.fs"}, {"in_Pos"});

  Tiny::Model verticemodel({"in_Pos"});
  verticemodel.bind<glm::vec4>("in_Pos", verticebuf);
 // verticemodel.SIZE = RES*RES;

  // Raw Surface Model

  // Create an Array of Indexing Points for the Triangles

  std::vector<glm::ivec4> indices;

  for(size_t i = 0; i < RES-1; i++)
  for(size_t j = 0; j < RES-1; j++){
    indices.emplace_back( (i)*RES + j, (i)*RES + j+1, (i+1)*RES + j, 1);
    indices.emplace_back( (i+1)*RES + j,  (i)*RES + j+1, (i+1)*RES + j+1, 1);
    indices.emplace_back( (i)*RES + j,  (i+1)*RES + j+1, (i+1)*RES + j, 1);
  }

  Tiny::Buffer indexbuf(indices);

  Tiny::Shader surfaceShader({"shader/surface.vs", "shader/surface.fs"}, {"in_Pos", "in_Index"}, {"vertices"});

  Tiny::Triangle triangle;
	Tiny::Instance triangleinstance(triangle);
  triangleinstance.bind<glm::vec4>(indexbuf);

	surfaceShader.bind("vertices", &verticebuf);

  // Compute Shaders for Computing Forces and Updating Positions

  // Position, Velocity, Force -> Acceleration

  int t = 0;

  std::vector velocityvec(2*RES*RES, glm::vec4(0));
  std::vector forcevec(2*RES*RES, glm::vec4(0));

  Tiny::Buffer velocitybuf(velocityvec);
  Tiny::Buffer forcebuf(forcevec);

  Tiny::Compute shift_f("shader/shift_f.cs", {"vertices", "velocity", "force"});
  shift_f.bind("vertices", &verticebuf);
  shift_f.bind("velocity", &velocitybuf);
  shift_f.bind("force", &forcebuf);

  Tiny::Compute shift_pv("shader/shift_pv.cs", {"vertices", "velocity", "force"});
  shift_pv.bind("vertices", &verticebuf);
  shift_pv.bind("velocity", &velocitybuf);
  shift_pv.bind("force", &forcebuf);

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white

  //  pointShader.use();
  //  pointShader.uniform("vp", cam::vp);
  //  verticemodel.render(GL_POINTS);

    surfaceShader.use();
    surfaceShader.uniform("vp", cam.vp());
    triangleinstance.render(GL_LINE_STRIP, indices.size());

	};

	//Execute the render loop
	Tiny::loop([&](){

    if(paused) return;

    shift_f.use();
    shift_f.uniform("size", RES*RES);
    shift_f.uniform("res", RES);
    shift_f.uniform("t", t);
    shift_f.uniform("rk", 0);
    shift_f.dispatch(RES/32, RES/32);

    shift_pv.use();
    shift_pv.uniform("size", RES*RES);
    shift_pv.uniform("res", RES);
    shift_pv.uniform("t", t);
    shift_pv.uniform("rk", 0);
    shift_pv.dispatch(RES/32, RES/32);

    t++;

	});

	Tiny::quit();

	return 0;

}

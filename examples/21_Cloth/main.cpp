#include <TinyEngine/TinyEngine>
#include <TinyEngine/camera>

const int RES = 150;

int main( int argc, char* args[] ) {


	//Initialize a Window
  Tiny::view.vsync = false;
  Tiny::view.antialias = 0;

  Tiny::view.pointSize = 2.0f;

	Tiny::window("GPU Cloth Simulation", 800, 800);

  glDisable(GL_CULL_FACE);

  cam::near = 0.01f;
  cam::far = 500.0f;
  cam::zoomrate = 15.0f;
  cam::init(250, cam::PROJ);

  bool paused = true;
	Tiny::view.interface = [&](){};
  Tiny::event.handler = [&](){

		cam::handler();

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

  cam::look = glm::vec3(RES/2, RES/2, 0);


  Buffer verticebuf(vertices);

  // Raw Point Model

  Shader pointShader({"shader/point.vs", "shader/point.fs"}, {"in_Pos"});

  Model verticemodel({"in_Pos"});
  verticemodel.bind<glm::vec4>("in_Pos", &verticebuf);
  verticemodel.SIZE = RES*RES;

  // Raw Surface Model

  // Create an Array of Indexing Points for the Triangles

  std::vector<glm::ivec4> indices;

  for(size_t i = 0; i < RES-1; i++)
  for(size_t j = 0; j < RES-1; j++){
    indices.emplace_back( (i)*RES + j, (i)*RES + j+1, (i+1)*RES + j, 1);
    indices.emplace_back( (i+1)*RES + j,  (i)*RES + j+1, (i+1)*RES + j+1, 1);
    indices.emplace_back( (i)*RES + j,  (i+1)*RES + j+1, (i+1)*RES + j, 1);
  }

  Buffer indexbuf(indices);

  Shader surfaceShader({"shader/surface.vs", "shader/surface.fs"}, {"in_Pos", "in_Index"}, {"vertices"});

  Triangle triangle;
	Instance triangleinstance(&triangle);
  triangleinstance.bind<glm::ivec4>("in_Index", &indexbuf);

	surfaceShader.bind<glm::vec4>("vertices", &verticebuf);

  // Compute Shaders for Computing Forces and Updating Positions

  // Position, Velocity, Force -> Acceleration

  int t = 0;

  std::vector<glm::vec4> velocityvec(4*RES*RES, glm::vec4(0));
  std::vector<glm::vec4> forcevec(4*RES*RES, glm::vec4(0));

  Buffer velocitybuf(velocityvec);
  Buffer forcebuf(forcevec);

  Compute shift("shader/shift.cs", {"vertices", "velocity", "force"});

  shift.bind<glm::vec4>("vertices", &verticebuf);
  shift.bind<glm::vec4>("velocity", &velocitybuf);
  shift.bind<glm::vec4>("force", &forcebuf);

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white

  //  pointShader.use();
  //  pointShader.uniform("vp", cam::vp);
  //  verticemodel.render(GL_POINTS);

    surfaceShader.use();
    surfaceShader.uniform("vp", cam::vp);
    triangleinstance.render(GL_LINE_STRIP);

	};

	//Execute the render loop
	Tiny::loop([&](){

    if(paused) return;

    shift.use();
    shift.uniform("size", RES*RES);
    shift.uniform("res", RES);
    shift.uniform("t", t);

		for(int i = 0; i < 1; i++){

			shift.uniform("mode", 0);
			shift.uniform("rk", i);
			shift.dispatch(1+(RES*RES)/1024);

			shift.uniform("mode", 1);
			shift.uniform("rk", i);
			shift.dispatch(1+(RES*RES)/1024);

		}



		/*


		shift.uniform("mode", 2);
		shift.dispatch(1+(RES*RES)/1024);

		shift.uniform("mode", 3);
		shift.dispatch(1+(RES*RES)/1024);

		shift.uniform("mode", 4);
		shift.dispatch(1+(RES*RES)/1024);

		*/

    t++;

	});

	Tiny::quit();

	return 0;

}

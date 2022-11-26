#include <TinyEngine/TinyEngine>

struct Triangle : Model {
	Buffer vert;
	Triangle():Model({"vert"}),
	vert({1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				1.0f, 0.0f, 0.0f, 0.0f}){
		bind<glm::vec4>("vert", &vert);
		SIZE = 4;
	}
};

int main( int argc, char* args[] ) {

//	srand(time(NULL));

	//Initialize a Window
	Tiny::view.pointSize = 5.0f;
	Tiny::view.antialias = 1;
	Tiny::view.vsync = false;
	Tiny::benchmark = true;
	Tiny::window("Delaunay Triangulation Texture", 800, 800);

	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE1);
	glEnable(GL_CLIP_DISTANCE2);

	// Create a random set of centroids

	const int N = 128;
	std::vector<glm::vec2> centroids;
	for(int i = 0; i < N; i++){
		glm::vec2 p;
		p.x = (float)(rand()%1000)/500.0f - 1.0f;
		p.y = (float)(rand()%1000)/500.0f - 1.0f;
		centroids.push_back(p);
	}

	std::sort(centroids.begin(), centroids.end(), [](glm::vec2& a, glm::vec2& b){
		return (glm::length(a) < glm::length(b));
	});

	/*
		Instance render a simplex, split by and indexing structure.

		Either render the (4) surfaces to get the label map,
		or render the (6) lines.

		What we really want to do though is avoid rendering whole primitives
		for points which are already behind a triangle.
	*/

	// Create the Simplex Model???

	// Basically, I want to instance render a triangle.

	/*
		Note: This is really bad because complexity is basically cubed.
		What we want to do instead is acknowledge that no single line can appear twice
		in the triangulation.

		This means instead it makes more sense to sort from outside in...
	*/

	/*
	std::vector<glm::ivec4> indices;
	for(int i = 0; i < N; i++){
		indices.emplace_back(i, (i+1)%N, (i+2)%N, 0);
	}
//	for(int j = i+1; j < N; j++)
//	for(int k = j+1; k < N; k++)
	Buffer indexbuf(indices);
	*/




	std::vector<glm::ivec4> indices;
	for(int i = 0; i < N; i++)
	for(int j = i+1; j < N; j++)
	for(int k = j+1; k < N; k++)
		indices.emplace_back(i, j, k, 0);
	Buffer indexbuf(indices);



	Triangle triangle;
	Instance triangleinstance(&triangle);
  triangleinstance.bind<glm::ivec4>("in_Index", &indexbuf);


	Buffer centroidbuf(centroids);

	Model centroidmesh({"in_Position"});
	centroidmesh.bind<glm::vec2>("in_Position", &centroidbuf);
	centroidmesh.SIZE = N;

	Shader pointShader({"shader/point.vs", "shader/point.fs"}, {"in_Position"});
	Shader surfaceShader({"shader/surface.vs", "shader/surface.fs"}, {"in_Position", "in_Index"}, {"centroids"});

	surfaceShader.bind<glm::vec4>("centroids", &centroidbuf);

	//Add the Event Handler
	Tiny::event.handler = [&](){
	};

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){
	};

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(0));	//Clear Screen to white

		surfaceShader.use();
		triangleinstance.render(GL_TRIANGLE_STRIP);
	//	triangleinstance.render(GL_LINE_STRIP);

	//	pointShader.use();
		//centroidmesh.render(GL_POINTS);

	};

	//Execute the render loop
	Tiny::loop([&](){
	});

	Tiny::quit();

	return 0;

}

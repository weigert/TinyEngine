#include <TinyEngine/TinyEngine>
#include <TinyEngine/image>
#include <piolib/helper/distribution>
#include <piolib/pointset/point>

int main( int argc, char* args[] ) {

  //Load the Point-Sets
  std::vector<point::Pointset> pointsets;
  point::Pointset pointset;
  point::Pointset colorset;

  int maxsize = 0;
  int curframe = 0;
  for(int i = 0; i < 30; i++){
    point::load(pointset, colorset, "resource/view"+std::to_string(i)+".txt");
    if(pointset.size() > maxsize) maxsize = pointset.size();
    pointsets.push_back(pointset);
  }


//  std::cout<<maxsize<<std::endl;
  maxsize = 8192/2;
//  maxsize = 3456;
  for(int i = 0; i < 30; i++)
  for(int j = pointsets[i].size(); j < maxsize; j++)
    pointsets[i].push_back(glm::vec3(0.0f));

//  std::cout<<maxsize<<std::endl;

  //Initialize the Compute Shader System
	Tiny::window("Example Window", 512, 512);
	Compute compute("shader.cs", {"pointsetA", "pointsetB", "probability"});

  std::vector<float> probability(maxsize*maxsize, 0.0f);
  compute.buffer("pointsetA", pointsets[curframe]);
  compute.buffer("pointsetB", pointsets[(curframe+1)%30]);
  compute.buffer("probability", probability);

//	compute.limits();

	//Add the Event Handler
	Tiny::event.handler = [&](){
	};

	//Set up an ImGUI Interface here
	Tiny::view.interface = [&](){
	};

	//Define the rendering pipeline
	Tiny::view.pipeline = [&](){

		Tiny::view.target(glm::vec3(1));	//Clear Screen to white

		compute.use();
    compute.uniform("size", glm::vec2(maxsize));
		compute.dispatch(glm::vec3(maxsize/32, maxsize/32, 1));

	};

	//Execute the render loop
	Tiny::loop([&](){

    curframe = (curframe + 1)%30;
    compute.buffer("pointsetA", pointsets[curframe]);
    compute.buffer("pointsetB", pointsets[(curframe+1)%30]);

		compute.retrieve("probability", probability);
		std::cout<<"Output0: "<<probability[0]<<std::endl;

	});

	Tiny::quit();

	return 0;

}

/*

Basically:

Try and see if computing a probability distribution is faster in one way or the other.

*/

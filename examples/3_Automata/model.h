//Data
const int SIZE = 100;
const int WIDTH = 1200, HEIGHT = 800;
bool data[WIDTH*HEIGHT] = {false};
bool paused = true;

//Setup of Data
void setup(){
  //Generate Random Data!
  srand(time(NULL));
  for(int i = 0; i < WIDTH*HEIGHT; i++){
    data[i] = (rand()%2 == 0)?1:0;
  }
};

// Event Handler
std::function<void()> eventHandler = [&](){

  // Pause and Unpause
  if(!Tiny::event.press.empty())
    if(Tiny::event.press.back() == SDLK_p)
      paused = !paused;

};

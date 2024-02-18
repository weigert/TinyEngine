//Data
const int SIZE = 100;
const int WIDTH = 1200, HEIGHT = 800;
bool data[WIDTH*HEIGHT] = {false};

//Setup of Data
void setup(){
  //Generate Random Data!
  srand(time(NULL));
  for(int i = 0; i < WIDTH*HEIGHT; i++){
    data[i] = (rand()%2 == 0)?1:0;
  }
};
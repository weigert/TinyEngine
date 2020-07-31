std::vector<glm::vec2> centroids;
std::vector<glm::vec2> offset;
float K = 1024; //That's a lot of polygons
float R = 4.0f*sqrt(4.0f/3.14159265f/K);
int NCOLOR = 1;


std::function<void()> eventHandler = [&](){

  if(Tiny::event.click[1]){
    glm::vec2 windowdim = glm::vec2(Tiny::view.WIDTH, Tiny::view.HEIGHT);
    glm::vec2 pos = 2.0f*glm::vec2(Tiny::event.mouse.x, Tiny::event.mouse.y)/windowdim-1.0f;

    if(sample::disc(centroids, R/2.0f, pos, glm::vec2(-1), glm::vec2(1)))
      offset.push_back(centroids.back());

    while(pow(NCOLOR, 3) < centroids.size())
      NCOLOR++;
  }

};

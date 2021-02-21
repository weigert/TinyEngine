namespace image {
using namespace glm;
using namespace std;

  SDL_Surface* load(string path){
    SDL_Surface* loaded = IMG_Load(path.c_str());
    if(loaded == NULL)
      printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    SDL_Surface* optimized = SDL_ConvertSurfaceFormat(loaded, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(loaded);
    return optimized;
  }

  void save(SDL_Surface* surface, string path){
    IMG_SavePNG(surface, path.c_str());
  }

  void save(Target target, string path){
    SDL_Surface *s = SDL_CreateRGBSurface(0, target.WIDTH, target.HEIGHT, 32, 0, 0, 0, 0);
    SDL_LockSurface(s);
    target.sample(s->pixels, vec2(0), vec2(target.WIDTH, target.HEIGHT));
    SDL_UnlockSurface(s);
    save(s, path);
    delete s;
  }

  SDL_Surface* make(function<vec4(int)> handle, vec2 size = vec2(Tiny::view.WIDTH, Tiny::view.HEIGHT)){
    SDL_Surface *s = SDL_CreateRGBSurface(0, size.x, size.y, 32, 0, 0, 0, 0);
    SDL_LockSurface(s);

    unsigned char* img_raw = (unsigned char*)s->pixels; //Raw Data

    for(int i = 0; i < size.x*size.y; i++){
      vec4 color = (handle)(i);  //Construct from Algorithm
      *(img_raw+4*i)    = (unsigned char)(255*color.x);
      *(img_raw+4*i+1)  = (unsigned char)(255*color.y);
      *(img_raw+4*i+2)  = (unsigned char)(255*color.z);
      *(img_raw+4*i+3)  = (unsigned char)(255*color.w);
    }

    SDL_UnlockSurface(s);
    return s;
  }

}

namespace image {

  SDL_Surface* load(std::string path){
    return IMG_Load(path.c_str());
  }

  void save(SDL_Surface* surface, std::string path){
    IMG_SavePNG(surface, path.c_str());
  }

  template<typename T>
  SDL_Surface* make(glm::vec2 size, T* data, std::function<glm::vec4(T)> handle){
    //Construct and Return the Surface
    SDL_Surface *s = SDL_CreateRGBSurface(0, size.x, size.y, 32, 0, 0, 0, 0);
    SDL_LockSurface(s);

    //Raw Data Pointer
    unsigned char* img_raw = (unsigned char*)s->pixels;

    for(int i = 0; i < size.x*size.y; i++){
      glm::vec4 color = handle(data[i]);
      *(img_raw+4*i)    = (unsigned char)(255*color.x);
      *(img_raw+4*i+1)  = (unsigned char)(255*color.y);
      *(img_raw+4*i+2)  = (unsigned char)(255*color.z);
      *(img_raw+4*i+3)  = (unsigned char)(255*color.w);
    }

    SDL_UnlockSurface(s);
    return s;
  }
};

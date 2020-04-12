namespace image {

  SDL_Surface* load(std::string path){
    return IMG_Load(path.c_str());
  }

  void save(SDL_Surface* surface, std::string path){
    IMG_SavePNG(surface, path.c_str());
  }

};

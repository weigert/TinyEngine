#ifndef TINYENGINE_IMAGE
#define TINYENGINE_IMAGE

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>

namespace image {
using namespace glm;
using namespace std;
using Handle = function<void()>;

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

  #ifdef TINYENGINE_UTILITIES

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

  #else

  SDL_Surface* make(function<vec4(int)> handle, vec2 size){
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

  #endif

  vec4 color(SDL_Surface* s, int x, int y){

    //Boundary Handling
    if(x >= s->w) return vec4(0,0,0,1);
    if(y >= s->h) return vec4(0,0,0,1);
    if(x < 0) return vec4(0,0,0,1);
    if(y < 0) return vec4(0,0,0,1);

  	SDL_PixelFormat *fmt = s->format;
  	Uint8 r,g,b,a;

  	SDL_LockSurface(s);
  	Uint32 p = (((Uint32*)s->pixels)[y*s->w+x]);
  	SDL_UnlockSurface(s);

    r = (Uint8)(((p & fmt->Rmask) >> fmt->Rshift) << fmt->Rloss);
    g = (Uint8)(((p & fmt->Gmask) >> fmt->Gshift) << fmt->Gloss);
    b = (Uint8)(((p & fmt->Bmask) >> fmt->Bshift) << fmt->Bloss);
    a = (Uint8)(((p & fmt->Amask) >> fmt->Ashift) << fmt->Aloss);

    return vec4(r,g,b,a);
  }

  vec4 color(SDL_Surface* s, float x, float y){
    return color(s, x*s->w, y*s->h);
  }


}

#endif

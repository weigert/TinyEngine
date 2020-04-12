namespace draw{

  /* Surface Constructor */

  //Define the color
  SDL_Surface* makeSurface(glm::vec2 d, CArray R, CArray G, CArray B, CArray A){
    //Construct and Return the Surface
    SDL_Surface *s = SDL_CreateRGBSurface(0, d.x, d.y, 32, 0, 0, 0, 0);
    SDL_LockSurface(s);

    //Create raw data pointer
    unsigned char *img_raw = (unsigned char*)s->pixels;

    for(int i = 0; i < d.x*d.y; i++){
      //Raw Pointer Stuff
      *(img_raw+4*i)    = (unsigned char)R[i].real();
      *(img_raw+4*i+1)  = (unsigned char)G[i].real();
      *(img_raw+4*i+2)  = (unsigned char)B[i].real();
      *(img_raw+4*i+3)  = (unsigned char)A[i].real();
    }

    SDL_UnlockSurface(s);
    return s;
  }
}

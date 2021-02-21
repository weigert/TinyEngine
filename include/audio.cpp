#include "audio.h"

bool Audio::init(){
  enabled = ( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) != -1 );
  return enabled;
}

void Audio::load(slist in){
  for (auto& e: in){
    Mix_Chunk* sound = Mix_LoadWAV( e.c_str() );
    if(sound != NULL)
      sounds[e] = sound;
    else std::cout<<"Failed to load audio file "<<e<<std::endl;
  }
}

void Audio::play(std::string sound){
  unprocessed.push_back(sounds[sound]);
}

bool Audio::quit(){
  for (auto& s: sounds)
    Mix_FreeChunk( s.second );
  Mix_CloseAudio();
  return true;
}

void Audio::process(){
  while(!unprocessed.empty()){
    if(unprocessed.back() != NULL)
      Mix_PlayChannel( -1, unprocessed.back(), 0 );
    else std::cout<<"Failed to play sound byte"<<std::endl;
    unprocessed.pop_back();
  }
}

#ifndef TINYENGINE_AUDIO
#define TINYENGINE_AUDIO

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdexcept>
#include <string>
#include <initializer_list>
#include <deque>
#include <unordered_map>

namespace Tiny {

class Audio{
public:

  using slist = std::initializer_list<std::string>;

  bool enabled = false;

  std::unordered_map<std::string, Mix_Chunk*> sounds;
  std::deque<Mix_Chunk*> unprocessed;

  bool init();
  bool quit();

  void load(slist in);
  void play(std::string sound);
  void process();
};

#endif

}
#ifndef TINYENGINE_AUDIO
#define TINYENGINE_AUDIO

class Audio{
public:
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

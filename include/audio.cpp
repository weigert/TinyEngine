/*
    WORK IN PROGRESS
*/

class Audio{
public:
  //Storage for unprocessed soundbytes
//  std::vector<SoundByte> unprocessed;

  //Vector containing the guy
  Mix_Chunk *med = NULL;
  Mix_Chunk *hit = NULL;

  bool init();
  bool cleanup();
  void process();
};

bool Audio::init(){
  //Intialize Interface
  if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 ) return false;

/*
  //Load the sound effects
  med = Mix_LoadWAV( "resource/audio/acoustic.wav" );
  hit = Mix_LoadWAV( "resource/audio/medium.wav" );

  //If there was a problem loading the sound effects
  if( med == NULL ) return false;
  if( hit == NULL ) return false;
*/

  //If everything loaded fine
  return true;
}

bool Audio::cleanup(){
  //Free the Chunks
  Mix_FreeChunk( med );
  Mix_FreeChunk( hit );

  //Close the Audio Interface
  Mix_CloseAudio();

  return true;
}

void Audio::process(){

  /*
  In the future, we can also save the position where the sound-effect was emitted
  -> to make sure that volume effects play a role.


  I need to be able to play sounds based on queues
  with a specific volume
  and maybe a bunch of other possible effects...
  */
  /*

  while(!unprocessed.empty()){
    if(unprocessed.back() != SOUND_NONE) Mix_PlayChannel( -1, hit, 0 );
    //Play the corresponding sound-effect and remove it from the unprocessed list.
    unprocessed.pop_back();
  }
  */
}

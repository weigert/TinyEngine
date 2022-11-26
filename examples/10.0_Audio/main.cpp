#include <TinyEngine/TinyEngine>

int main( int argc, char* args[] ) {

	Tiny::audio.init();
	Tiny::audio.load({"acoustic.wav"});
	Tiny::audio.play("acoustic.wav");

	Tiny::loop([&](){
		//This waits until the program quits...
	});

	Tiny::quit();

	return 0;
}

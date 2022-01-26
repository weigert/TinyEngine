#include <TinyEngine/TinyEngine>

#include "compute.h"

int main( int argc, char* args[] ){

	Tiny::init(); 					//Initialize the Headless OpenGL Context

	//increment();						//Perform Increment Test
	accumulate(); 					//Perform Reduction Test
	//gausstransform();				//Perform Gauss Transform Test
	//matrixmatrix();

	Tiny::quit();						//Elegantly Quit TinyEngine

	return 0;

}

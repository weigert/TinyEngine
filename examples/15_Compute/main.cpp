#include <TinyEngine/TinyEngine>

#define SIZE 8192
#define ITER 10000

#include "tests.h"

int main( int argc, char* args[] ){

	Tiny::init(); //Initialize the Headless OpenGL Context

	//Increment Test
	std::cout<<"Increment Test: "<<std::endl;
	increment();

	std::cout<<"Reduction Test: "<<std::endl;
	reduce();


	Tiny::quit();

	return 0;

}

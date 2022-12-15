#include <TinyEngine/TinyEngine>
#include "parse.h"
#include "yaml.h"

struct StructA {

    int a; 
    double b;

    StructA(){
        a << yaml::key("a");
        b << yaml::key("b");
    }

};

int main( int argc, char* args[] ) {

	StructA a;
	a << yaml::key("root");

	int t = 0;
	t << yaml::key("t");

	for(auto& i: yaml::index){
		std::cout<<i<<std::endl;
	}

	if(!yaml::load(&t, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"t = "<<t<<std::endl;

	return 0;

}

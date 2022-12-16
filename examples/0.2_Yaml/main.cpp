#include <TinyEngine/TinyEngine>
//#include "parse.h"
#include "yaml.h"

struct StructA {

    int a; 
    double b;
    float c;
    double d;

    StructA(){
        b << yaml::key("b");
        d << yaml::key("d");
        a << yaml::key("a");
    }

};

struct StructB {

    StructA A;
    char c;

    StructB(){
        c << yaml::key("c");
        A << yaml::key("A");
    }

};

struct StructC {

    StructB b;
    char g;

    StructC(){
        g << yaml::key("g");
        b << yaml::key("b");
    }

};

int main( int argc, char* args[] ) {

	// Integer Loading

	int i = 0;
	i << yaml::key("i");

	if(!yaml::load(&i, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"i = "<<i<<std::endl;

	// Float Loading

	float f = 0.0;
	f << yaml::key("f");

	if(!yaml::load(&f, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"f = "<<f<<std::endl;

	// Double Loading

	double d = 0.0;
	d << yaml::key("d");

	if(!yaml::load(&d, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"d = "<<d<<std::endl;

	// Struct Loading

	StructA a;
	a << yaml::key("root");

	if(!yaml::load(&a, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"a.a = "<<a.a<<std::endl;
	std::cout<<"a.b = "<<a.b<<std::endl;
	std::cout<<"a.d = "<<a.d<<std::endl;

	// Nested Struct Loading

	StructB b;
	b << yaml::key("broot");

	if(!yaml::load(&b, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"b.c = "<<b.c<<std::endl;
	std::cout<<"b.A.a = "<<b.A.a<<std::endl;

	// Double-Nested Struct

	StructC c;
	c << yaml::key("c");
	std::cout<<"YAML KEY: "<<yaml::key(&c)<<std::endl;

	if(!yaml::load(&c, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"c.b.c = "<<c.b.c<<std::endl;
	std::cout<<"c.g = "<<c.g<<std::endl;

	return 0;

}

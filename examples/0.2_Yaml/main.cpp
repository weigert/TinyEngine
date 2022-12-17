#include <TinyEngine/TinyEngine>
#include "../../include/helpers/yaml.h"

#include "config.h"

int main( int argc, char* args[] ) {

	// Basic Types

	// Integer Loading

	i << yaml::key("integer");
	if(!yaml::load(&i, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<yaml::find(&i)<<" = "<<i<<std::endl;

	// Float Loading

	f << yaml::key("float");
	if(!yaml::load(&f, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<yaml::find(&f)<<" = "<<f<<std::endl;

	// Double Loading

	d << yaml::key("double");
	if(!yaml::load(&d, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<yaml::find(&d)<<" = "<<d<<std::endl;

	// Char

	c << yaml::key("char");
	if(!yaml::load(&c, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<yaml::find(&c)<<" = "<<c<<std::endl;

	// String

	s << yaml::key("string");
	if(!yaml::load(&s, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<yaml::find(&s)<<" = "<<s<<std::endl;

	// Structs / Nested Structs

	// Struct Loading

	A << yaml::key("StructA");
	if(!yaml::load(&A, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<"a.a = "<<A.a<<std::endl;
	//std::cout<<"a.b = "<<A.b<<std::endl;
	//std::cout<<"a.d = "<<A.d<<std::endl;

	// Nested Struct Loading

	B << yaml::key("StructB");
	if(!yaml::load(&B, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	//std::cout<<"b.c = "<<B.c<<std::endl;
	//std::cout<<"b.A.a = "<<B.A.a<<std::endl;

	// Double-Nested Struct

	C << yaml::key("StructC");
	if(!yaml::load(&C, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	//std::cout<<"c.b.c = "<<C.b.c<<std::endl;
	//std::cout<<"c.g = "<<C.g<<std::endl;

	// Vector

	v << yaml::key("vector");
	if(!yaml::load(&v, "config.yaml")){
		std::cout<<"Failed to load config.yaml"<<std::endl;
		exit(1);
	}

	std::cout<<"vector:"<<std::endl;
	for(auto& vec: v){
		std::cout<<"  "<<vec<<std::endl;
	}

	return 0;

}

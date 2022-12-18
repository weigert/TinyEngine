#include <TinyEngine/TinyEngine>
#include "../../include/helpers/yaml.h"

#include "config.h"

int main( int argc, char* args[] ) {

	/*

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

	// GLM Vectors

	glm::ivec3 vec3;
	vec3 << yaml::key("vec3");
	if(!yaml::load(&vec3, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	// GLM Matrices

	glm::mat2 mat2;
	mat2 << yaml::key("mat2");
	if(!yaml::load(&mat2, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	std::cout<<"mat2"<<std::endl;
	std::cout<<"  "<<mat2[0][0]<<std::endl;
	std::cout<<"  "<<mat2[0][1]<<std::endl;
	std::cout<<"  "<<mat2[1][0]<<std::endl;
	std::cout<<"  "<<mat2[1][1]<<std::endl;


	glm::mat3 mat3;
	mat3 << yaml::key("mat3");
	if(!yaml::load(&mat3, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	std::cout<<"mat3"<<std::endl;
	std::cout<<"  "<<mat3[0][0]<<std::endl;
	std::cout<<"  "<<mat3[0][1]<<std::endl;
	std::cout<<"  "<<mat3[1][0]<<std::endl;
	std::cout<<"  "<<mat3[1][1]<<std::endl;

	// Structs / Nested Structs

	// Struct Loading

	A << yaml::key("StructA");
	if(!yaml::load(&A, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	std::cout<<"a.a = "<<A.a<<std::endl;
	std::cout<<"a.b = "<<A.b<<std::endl;
	std::cout<<"a.d = "<<A.d<<std::endl;

	// Nested Struct Loading

	B << yaml::key("StructB");
	if(!yaml::load(&B, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;
	std::cout<<"b.c = "<<B.c<<std::endl;
	std::cout<<"b.A.a = "<<B.A.a<<std::endl;

	*/

	// Double-Nested Struct

	C << yaml::key("StructC");
	if(!yaml::load(&C, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"c.B.c = "<<C.B.c<<std::endl;
	std::cout<<"c.g = "<<C.g<<std::endl;

	/*

	// Arrays

	farr << yaml::key("float_array");
	if(!yaml::load(&farr, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"farr:"<<std::endl;
	for(size_t i = 0; i < 5; i++)
		std::cout<<"  "<<farr[i]<<std::endl;

	// Array of Struct

	structarr << yaml::key("struct_array");
	if(!yaml::load(&structarr, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"structarr:"<<std::endl;
	for(size_t i = 0; i < 3; i++)
		std::cout<<"  "<<structarr[i].a<<std::endl;

	// Vector

	v << yaml::key("vector");
	if(!yaml::load(&v, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"vector:"<<std::endl;
	for(auto& vec: v){
		std::cout<<"  "<<vec<<std::endl;
	}

	// Set

	sset << yaml::key("sset");
	if(!yaml::load(&sset, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"sset:"<<std::endl;
	for(auto& s: sset){
		std::cout<<"  "<<s<<std::endl;
	}

	iset << yaml::key("iset");
	if(!yaml::load(&iset, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"iset:"<<std::endl;
	for(auto& i: iset){
		std::cout<<"  "<<i<<std::endl;
	}

	// Map

	iimap << yaml::key("iimap");
	if(!yaml::load(&iimap, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"iimap:"<<std::endl;
	for(auto& ii: iimap){
		std::cout<<"  "<<ii.first<<" "<<ii.second<<std::endl;
	}

	ismap << yaml::key("ismap");
	if(!yaml::load(&ismap, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"ismap:"<<std::endl;
	for(auto& ii: ismap){
		std::cout<<"  "<<ii.first<<" "<<ii.second<<std::endl;
	}

	simap << yaml::key("simap");
	if(!yaml::load(&simap, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"simap:"<<std::endl;
	for(auto& ii: simap){
		std::cout<<"  "<<ii.first<<" "<<ii.second<<std::endl;
	}

	// Struct of Vector

	vecstruct << yaml::key("vector_struct");
	if(!yaml::load(&vecstruct, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"vector_struct:"<<std::endl;
	for(auto& ii: vecstruct.v){
		std::cout<<"  "<<ii<<std::endl;
	}
	
	// Vector of Vector

	vv << yaml::key("vv");
	if(!yaml::load(&vv, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"vector of vector:"<<std::endl;
	for(auto& v: vv){
		for(auto& vec: v){
			std::cout<<"  "<<vec<<std::endl;
		}
		std::cout<<std::endl;
	}

	// Map of Vector

	miv << yaml::key("miv");
	if(!yaml::load(&miv, "config.yaml"))
		std::cout<<"Failed to load config.yaml"<<std::endl;

	std::cout<<"map of vector:"<<std::endl;
	for(auto& m: miv){
		std::cout<<m.first<<std::endl;
		for(auto& vec: m.second){
			std::cout<<"  "<<vec<<std::endl;
		}
	}

	*/

	return 0;

}

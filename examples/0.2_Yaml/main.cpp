#include <TinyEngine/TinyEngine>
#include"yaml.hpp"

int main( int argc, char* args[] ) {

	// A struct of this type will always have this key

	struct B: yaml::node<"struct B: test"> {
		B(){ yaml::done(); }
	};

	// This struct has a type which has a strict key,
	// 	in other words B has a fixed key which cannot be overwritten.

	struct A: yaml::node<"struct A"> {
		A(){ yaml::done(); }

		int a 	= yaml::val<"a">(1);
		float b = yaml::val<"b">(1.0f);

		B bb;
	} a;

	// This struct instead has its key determined
	// 	at compile time, but not as a strict derived type
	//  which forces it to a specific key

	struct C {
		C(){ yaml::done(); }
		char c = yaml::val<"c">('c');
	};

	C c0 = yaml::node<"C0">{};
	C c1 = yaml::node<"C1">{};


	// Lets try full compile time resolution???



	// Other stuff

	//int j = yaml::val<"j">(1);

	//yaml::key<"A"> midkey_a;
	//yaml::key<"B"> midkey_b;
	//yaml::key<"C"> midkey_c;
	//yaml::key<"D"> midkey_d;

	return 0;

}

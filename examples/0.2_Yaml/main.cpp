#include <TinyEngine/TinyEngine>
#include"yaml.hpp"

int main( int argc, char* args[] ) {

	// A struct of this type will always have this key

	struct B: yaml::obj<"struct B"> {
		B(){ yaml::done(); }
	};

	// This struct has a type which has a strict key,
	// 	in other words B has a fixed key which cannot be overwritten.

	struct A: yaml::obj<"struct A"> {
		A(){ yaml::done(); }

		int a 	= yaml::val<"a", int>(1);
		float b = yaml::val<"b", float>(1.0f);

		B bb;
	} a;

	// This struct instead has its key determined
	// 	at compile time, but not as a strict derived type
	//  which forces it to a specific key

	struct C {
		C(){ yaml::done(); }
		char c = yaml::val<"c", char>('c');
	};

	C c0 = yaml::obj<"C0">{};
	C c1 = yaml::obj<"C1">{};

	// Full Compile Time Resolution

	struct D: yaml::obj<"struct D"> {
		D(){ D::done(); }
		size_t n = D::val<"n">(1);
		char c = D::val<"c">('b');
	//	C c0 = yaml::obj<"C0">{};
};

	D d0;
	D d1;

//	std::cout<<"SUBSIZE "<<D.sub.size()<<std::endl;

	// Other stuff

	int j = yaml::val<"j", int>(1);

	//yaml::key<"A"> midkey_a;
	//yaml::key<"B"> midkey_b;
	//yaml::key<"C"> midkey_c;
	//yaml::key<"D"> midkey_d;

	return 0;

}

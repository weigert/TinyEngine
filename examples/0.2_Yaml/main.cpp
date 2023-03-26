#include <TinyEngine/TinyEngine>
#include"yaml.hpp"

int main( int argc, char* args[] ) {

	// Abstract Object Models

	struct A: ctom::obj_impl<
		ctom::ref_val<"sub-sub-int", int>,
		ctom::ref_val<"sub-sub-float", float>,
		ctom::ref_val<"sub-sub-double", double>
	>{};

	struct B: ctom::obj_impl<
		ctom::ref_obj<"sub-obj", A>
	>{};

	struct T: ctom::obj_impl <
		ctom::ref_val<"valA", int>,
		ctom::ref_arr<"array", int>,
		ctom::ref_obj<"object", B>,
		ctom::ref_val<"valB", int>
	>{};

	// Concrete Model Translation

	struct T_derived: T {
		int t = T::val<"valA">(10);
		int a;
		B b = T::obj<"object">(B{});
	} Tval;

	/*
			Now I need to write an actual proper printer...
			And arrays need to work too...
			And I need to get the type copying to also work,
			i.e. a parser thingy.
	*/
	ctom::print<T>();

	return 0;

}

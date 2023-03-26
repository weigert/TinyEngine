#include <type_traits>
#include <iomanip>
#include <array>

namespace ctom {

/*
ctom.hpp
compile-time object model
author: Nicholas McDonald 2023

this namespace lets you define a compile-time object model using template
meta-programming techniques. The object-model allows for fast (un)marshalling
of various data formats, and is similar to semantic tags in golang.

the original intent was a type-safe compile-time yaml parser.

================================================================================
                        Implementation Details
================================================================================
*/

// Constant Expression String
//  Lets us use a string-literal as a template parameter

template<size_t N> struct constexpr_string {

  char value[N + 1] = {};
  constexpr constexpr_string(const char (&_value)[N+1]) {
      for (size_t n = 0; n < N; ++n)
        value[n] = _value[n];
  }

  constexpr operator char const*() const { return value; }
  constexpr char operator[](size_t n) const noexcept {
		return value[n];
	}
  constexpr size_t size() const noexcept {
		return N;
	}
  constexpr const char * begin() const noexcept {
    return value;
  }
  constexpr const char * end() const noexcept {
    return value + size();
  }

};
template<unsigned N> constexpr_string(char const (&)[N]) ->constexpr_string<N-1>;

// Object in Set

template <typename T, typename... List>
struct is_contained;

template <typename T>
struct is_contained<T> {
  static constexpr bool value = false;
};

template <typename T, typename Head, typename... Tail>
struct is_contained<T, Head, Tail...>{
  static constexpr bool value =
    std::is_same<T, Head>::value || is_contained<T, Tail...>::value;
};

// Unique Key in Key-Set

template <typename... List>
struct is_unique;

template <>
struct is_unique<> {
    static constexpr bool value = true;
};

template <typename Head, typename... Tail>
struct is_unique<Head, Tail...>{
  static constexpr bool value = !is_contained<Head, Tail...>::value && is_unique<Tail...>::value;
};

// Tuple Indexer

template <class T, size_t N, class... Args>
struct tuple_index {
    static constexpr size_t value = N;
};

template <class T, size_t N, class... Args>
struct tuple_index<T, N, T, Args...> {
    static constexpr size_t value = N;
};

template <class T, size_t N, class U, class... Args>
struct tuple_index<T, N, U, Args...> {
    static constexpr size_t value = tuple_index<T, N + 1, Args...>::value;
};

/*
================================================================================
                        Template Meta Object Model
================================================================================
*/

// Basic Ref-Types

struct ref_base{};
struct val_base{
  static constexpr const char * type = "val";
};
struct arr_base{
  static constexpr const char * type = "arr";
};
struct obj_base{
  static constexpr const char * type = "obj";
};

// Base-Type Concepts
//  Lets us defined templated derived types
//  and still easily restrict parameters

template<typename T>
concept ref_type = std::derived_from<T, ref_base>;
template<typename T>
concept val_type = std::derived_from<T, val_base>;
template<typename T>
concept arr_type = std::derived_from<T, arr_base>;
template<typename T>
concept obj_type = std::derived_from<T, obj_base>;

// References to Base-Type Implementations
//  Acts as an identifier for a ref to
//  specific type implementations

template<constexpr_string ref, val_type V>
struct ref_val_impl: ref_base {};

template<constexpr_string Key, arr_type T>
struct ref_arr_impl: ref_base {};

template<constexpr_string Key, obj_type T>
struct ref_obj_impl: ref_base {};

// Node-Type Declarations
//  Structs which are templated by a specific ref type
//  give us specific node types (with a required ref)

template<ref_type ref> struct node {};

template<constexpr_string ref, val_type T>
struct node<ref_val_impl<ref, T>> {
  static constexpr const char* type = "val";
  static constexpr char const* key = ref;
  T val;

  void print(){
    std::cout<<key<<": ";//<<value<<std::endl;
    val.print();
  }
};

template<constexpr_string ref, arr_type T>
struct node<ref_arr_impl<ref, T>> {
  static constexpr const char* type = "arr";
  static constexpr char const* key = ref;
  T arr;

  void print(){
    std::cout<<key<<": ";//<<value<<std::endl;
    arr.print();
  }
};

template<constexpr_string ref, obj_type T>
struct node<ref_obj_impl<ref, T>> {
  static constexpr const char* type = "obj";
  static constexpr char const* key = ref;
  T obj;

  void print(){
    std::cout<<key<<": "<<std::endl;
    obj.print();
  }
};

// Node Type Aliases

template<constexpr_string ref, val_type T>
using node_val = node<ref_val_impl<ref, T>>;

template<constexpr_string ref, arr_type T>
using node_arr = node<ref_arr_impl<ref, T>>;

template<constexpr_string ref, obj_type T>
using node_obj = node<ref_obj_impl<ref, T>>;

/*
================================================================================
                              Implementations
================================================================================
*/

template<typename V>
concept is_value =
    std::is_same_v<V, bool>
||  std::is_same_v<V, char>
||  std::is_same_v<V, int>
||  std::is_same_v<V, long>
||  std::is_same_v<V, size_t>
||  std::is_same_v<V, float>
||  std::is_same_v<V, double>;

template<is_value V>
struct val_impl: val_base {

  V value = 5;

  void print(){
    std::cout<<value<<std::endl;
  }

};

template<constexpr_string ref, is_value V>
using ref_val = ref_val_impl<ref, val_impl<V>>;

// Array

template<typename T>
struct arr_impl: arr_base {
  // std::vector<T> values;

  void print(){
    std::cout<<"ARRAY IMPL"<<std::endl;
  }

};

template<constexpr_string ref, typename T>
using ref_arr = ref_arr_impl<ref, arr_impl<T>>;

// Object

template<constexpr_string Key, obj_type T>
using ref_obj = ref_obj_impl<Key, T>;




// Reference Forwarding

template<ref_type... refs>
struct ref_forward {
  static void dof();
};

template<ref_type... refs>
void ref_forward<refs...>::dof(){
  std::cout<<std::tuple_element_t<0, std::tuple<node<refs>...>>::key<<std::endl;
  std::cout<<std::tuple_element_t<1, std::tuple<node<refs>...>>::key<<std::endl;
}








template<ref_type... refs>
struct obj_impl: obj_base {

  //static_assert(unique_key_set<refs...>::value)

  std::tuple<node<refs>...> nodes;

  // Somehow I need to get the arguments of the tuple?

//
  //
  //static void for_refs(T& t);

  //static void for_args(refs const&... args){

  //}

  static ref_forward<refs...> _ref_forward;


  template<ref_type ref> struct index {
    static constexpr size_t value = tuple_index<ref, 0, refs...>::value;
  };

  template<ref_type ref>
  node<ref>& get() {
    return std::get<index<ref>::value>(nodes);
  }

  // Assign a value!
  template<constexpr_string ref, typename V>
  V val(const V& v){
    static_assert(is_value<V>, "type is not a value type");
    static_assert(is_contained<ref_val<ref, V>, refs...>::value, "key for yaml::val does not exist in yaml::obj");
    node_val<ref, val_impl<V>>& node = get<ref_val<ref, V>>();
    node.val.value = v;
    return std::move(v);
  }

  template<constexpr_string ref, typename T>
  T obj(const T& v){
    static_assert(obj_type<T>, "type is not a derived type of yaml::obj");
    static_assert(is_contained<ref_obj<ref, T>, refs...>::value, "key for yaml::obj does not exist in yaml::obj");
    node<ref_obj<ref, T>>& node = get<ref_obj<ref, T>>();
    node.obj = v;
    return std::move(v);
  }

  void print(){

    std::apply([](auto&&... args){
      (args.print(), ...);
    //
    //  ((std::cout << args.type << " \"" << args.key << "\"\n"), ...);
    }, nodes);

  }

};

template<ref_type... refs>
ref_forward<refs...> obj_impl<refs...>::_ref_forward;

/*
================================================================================
                          Marshalling / Unmarshalling
================================================================================
*/

// Regular Object-Model Dump
//  Note: This is compile-time static, meaning that values don't matter.


template<constexpr_string ref, val_type T>
void print(node<ref_val_impl<ref, T>>& node, size_t shift){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<val_base::type<<": ";
  std::cout<<ref<<"\n";
}

template<constexpr_string ref, arr_type T>
void print(node<ref_arr_impl<ref, T>>& node, size_t shift){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<arr_base::type<<": ";
  std::cout<<ref<<"\n";
}

template<constexpr_string ref, obj_type T>
void print(node<ref_obj_impl<ref, T>>& node, size_t shift){
  for(size_t s = 0; s < shift; s++) std::cout<<"  ";
  std::cout<<obj_base::type<<": ";
  std::cout<<ref<<"\n";

  std::apply([&](auto&&... args){
    (ctom::print(args, shift+1), ...);
  }, node.obj.nodes);
}

template<obj_type T>
void print(T& obj){
  std::apply([](auto&&... args){
    (ctom::print(args, 0), ...);
  }, obj.nodes);
}


// Fully Static Version!
//  Not possible because partial specialization is not allowed. great.

template<typename T>
struct printer {
  static void print(size_t shift = 0);
};

template<constexpr_string ref, val_type T>
struct printer<node<ref_val_impl<ref, T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<val_base::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, arr_type T>
struct printer<node<ref_arr_impl<ref, T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<arr_base::type<<": ";
    std::cout<<ref<<"\n";
  }
};

template<constexpr_string ref, obj_type T>
struct printer<node<ref_obj_impl<ref, T>>>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    std::cout<<obj_base::type<<": ";
    std::cout<<ref<<"\n";

  //  T::for_refs();
  //  T obj;
  //  std::apply([&](auto&&... args){
  //    (ctom::print(args, shift+1), ...);
  //  }, obj.nodes);

  }
};

//template<ref_type... refs>
//struct obj_impl: obj_base

template<obj_type T>
struct printer<T>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    T obj;

    T::_ref_forward.dof();

  //  T::for_refs();
  //  std::apply([&](auto&&... args){
  //    (ctom::print(args, shift), ...);
  //  }, obj.nodes);

  }
};

/*
template<obj_type T>
struct printer<T>{
  static void print(size_t shift = 0){
    for(size_t s = 0; s < shift; s++) std::cout<<"  ";
    T obj;
    std::apply([&](auto&&... args){
      (ctom::print(args, shift), ...);
    }, obj.nodes);

  }
};
*/

template<typename T>
void print(){
  printer<T>::print();
}

}

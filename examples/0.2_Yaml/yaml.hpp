/*
ctom.hpp

compile-time object model

minimal yaml parser
idea: what if instead of doing it yaml -> object model -> into struct,
we instead do struct -> object model -> yaml regex?

just an idea.
either way, we need a method to create structs in an easy way and load stuff in.

basically a compile-time yaml parser.
the struct is constructed first, then it can parse something.
that is all.
*/

#include <type_traits>
#include <iomanip>
#include <array>

namespace ctom {

// constant expression string,
//  allows us to use this as a template parameter with
//  constexpr member values

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

/*
================================================================================
                                Helper Stuff
================================================================================
*/

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
struct val_base{};
struct arr_base{};
struct obj_base{};

// Base-Type Concepts

template<typename T>
concept ref_type = std::derived_from<T, ref_base>;
template<typename T>
concept val_type = std::derived_from<T, val_base>;
template<typename T>
concept arr_type = std::derived_from<T, arr_base>;
template<typename T>
concept obj_type = std::derived_from<T, obj_base>;

// References to Base-Type Implementations

template<constexpr_string ref, val_type V>
struct ref_val_impl: ref_base {};

template<constexpr_string Key, arr_type T>
struct ref_arr_impl: ref_base {};

template<constexpr_string Key, obj_type T>
struct ref_obj_impl: ref_base {};

// Node-Type Declarations

template<ref_type ref> struct node {};

template<constexpr_string ref, val_type T>
using node_val = node<ref_val_impl<ref, T>>;

template<constexpr_string ref, arr_type T>
using node_arr = node<ref_arr_impl<ref, T>>;

template<constexpr_string ref, obj_type T>
using node_obj = node<ref_obj_impl<ref, T>>;

// Node Type Implementations

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

template<ref_type... refs>
struct obj_impl: obj_base {

  //static_assert(unique_key_set<refs...>::value)

  std::tuple<node<refs>...> nodes;

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

}

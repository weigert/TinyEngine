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
                                Yaml Nodes
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

// Basic Type Constraints

template<typename V>
concept value_type =
    std::is_same_v<V, bool>
||  std::is_same_v<V, char>
||  std::is_same_v<V, int>
||  std::is_same_v<V, long>
||  std::is_same_v<V, size_t>
||  std::is_same_v<V, float>
||  std::is_same_v<V, double>;

// Let's try something new

struct key_base{};

template<typename T>
concept is_key = std::derived_from<T, key_base>;

// Note: Statically Assert if Two Template Arguments are the same!
//  Not allowed!

//template<constexpr_string... Keys>
//struct unique_key_set{
//  static constexpr bool value = is_unique<new_key<Keys>...>::value;
//};

// Specific, Distinct Types of Keys!

template<constexpr_string Key, typename V>
struct val_key: key_base {};

template<constexpr_string Key, typename T>
struct arr_key: key_base {};

// Specific Types derived from specific keys

template<is_key key>
struct node {};

template<constexpr_string Key, typename V>
using node_val = node<val_key<Key, V>>;

template<constexpr_string Key, typename V>
struct node<val_key<Key, V>> {
  static constexpr const char* type = "val";
  static constexpr char const* key = Key;
  V value;

  void print(){
    std::cout<<key<<": "<<value<<std::endl;
  }
};

template<constexpr_string Key, typename T>
using node_arr = node<arr_key<Key, T>>;

template<constexpr_string Key, typename T>
struct node<arr_key<Key, T>> {
  static constexpr const char* type = "arr";
  static constexpr char const* key = Key;
  // std::vector<T> values;

  void print(){
    std::cout<<key<<": "<<std::endl;
  }
};















struct obj_base{};

template<typename T>
concept is_obj = std::derived_from<T, obj_base>;

// This is the basic object type,
// meaning that it is templated by a bunch of keys.

template<constexpr_string Key, is_obj T>
struct obj_key: key_base {};

//template<is_key... Keys>
//struct new_obj;

template<constexpr_string Key, is_obj T>
struct node<obj_key<Key, T>> {
  static constexpr const char* type = "obj";
  static constexpr char const* key = Key;
  T obj;

  void print(){
    std::cout<<key<<": "<<std::endl;
    obj.print();
  }

};

template<constexpr_string Key, typename T>
using node_obj = node<obj_key<Key, T>>;

template<is_key... Keys>
struct new_obj: obj_base {

  //static_assert(unique_key_set<Keys...>::value)

  std::tuple<node<Keys>...> nodes;

  template<is_key Key> struct index {
    static constexpr size_t value = tuple_index<Key, 0, Keys...>::value;
  };

  template<is_key Key>
  node<Key>& get() {
    return std::get<index<Key>::value>(nodes);
  }

  // Assign a value!
  template<constexpr_string Key, typename V>
  V val(const V& v){
    static_assert(value_type<V>, "type is not a value type");
    static_assert(is_contained<val_key<Key, V>, Keys...>::value, "key for yaml::val does not exist in yaml::obj");
//    std::cout<<"INDEX: "<<index<val_key<Key>>::value<<std::endl;
    node_val<Key, V>& node = get<val_key<Key, V>>();
    node.value = v;
    return std::move(v);
  }

  template<constexpr_string Key, typename T>
  T obj(const T& v){
    static_assert(is_obj<T>, "type is not a derived type of yaml::obj");
    static_assert(is_contained<obj_key<Key, T>, Keys...>::value, "key for yaml::obj does not exist in yaml::obj");
//    std::cout<<"INDEX: "<<index<val_key<Key>>::value<<std::endl;
//    node<val_key<Key>> node = get<val_key<Key>>();
//    node<val_key<Key>> t = std::get<>(nodes);

    node<obj_key<Key, T>>& node = get<obj_key<Key, T>>();
  //  node_obj<Key, T> node =

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

// Print the Abstract Syntax Tree


template<constexpr_string Key, typename V>
static void yaml(node_val<Key, V> node){
  node.print();
}

template<constexpr_string Key, typename V>
static void yaml(node_arr<Key, V> node){
  node.print();
}

template<constexpr_string Key, typename T>
static void yaml(node_obj<Key, T> node){
  node.print();
}

//using node_val = node<val_key<Key>>;

template<is_key... Keys>
static void yaml(new_obj<Keys...>& obj){
  std::apply([](auto&&... args){
    ((yaml(args)), ...);
  //
  //  ((std::cout << args.type << " \"" << args.key << "\"\n"), ...);
  }, obj.nodes);
}

}

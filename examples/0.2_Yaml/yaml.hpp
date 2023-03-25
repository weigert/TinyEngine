/*
yaml.hpp
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

namespace yaml {

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

// yaml basic derivd types
//

// basic yaml key


static size_t shift = 0;

template<constexpr_string T>
struct key {

  static constexpr char const* _key = T;
  key(){
    for(size_t i = 0; i < shift; i++)
      std::cout<<"  ";
    std::cout<<_key<<std::endl;
  }

  // we can now do compile time assertion that the key is actually valid!

};

/*
================================================================================
                                Yaml Nodes
================================================================================
*/

// Nodes

struct node{};

// Node-Type: Value

struct val_base: node{};

template<typename T>
concept is_val = std::derived_from<T, val_base>;

template<typename V>
concept value_type =
    std::is_same_v<V, bool>
||  std::is_same_v<V, char>
||  std::is_same_v<V, int>
||  std::is_same_v<V, long>
||  std::is_same_v<V, size_t>
||  std::is_same_v<V, float>
||  std::is_same_v<V, double>;

template<constexpr_string Key, value_type V>
struct val: val_base {

  key<Key> _key;
  V value;

  val(const V& v){}
  constexpr operator V() const { return std::move(value); }

};

// Node-Type: Object

struct obj_base{

  std::vector<node> sub;
  
  template<constexpr_string SubKey, value_type V>
  constexpr V val(const V& v){
    yaml::val<SubKey, V> _subval(v);
    sub.push_back(_subval);
    return _subval;
  }

};

template<typename T>
concept is_obj = std::derived_from<T, obj_base>;

template<constexpr_string Key>
struct obj: obj_base {

  key<Key> _key;

  obj(){
    shift++;
  }

  // Runtime Assignment:
  //  Possible for non-node types or identical node types

  template<typename V> requires(!is_obj<V> && !value_type<V>)
  constexpr operator V() const { return V{}; }

  template<typename V> requires(std::derived_from<V, obj<Key>>)
  constexpr operator V() const { return V{}; }

  static void done(){
//    std::cout<<"DONE"<<std::endl;
    shift--;
  }

  // Sub-Node Constructors

};





static void done(){
//  std::cout<<"DONE"<<std::endl;
  shift--;
}

// Print the Abstract Syntax Tree

static void ast(){
  std::cout<<"PRINTING AST:"<<std::endl;
}

}

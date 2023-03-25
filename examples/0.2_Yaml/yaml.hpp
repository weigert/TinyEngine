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

template<constexpr_string T>
struct key {

  static constexpr char const* _key = T;
  key(){
    std::cout<<_key<<std::endl;
  }

  // we can now do compile time assertion that the key is actually valid!

};

// Nodes

struct base{};

template<constexpr_string T>
struct node;

template<typename T>
concept is_node = std::derived_from<T,base>;

template<constexpr_string T>
struct node : base {

  key<T> _key;
  node(){}

  // We can't assign to classes derived from base!

  template<typename K> requires(!is_node<K>)
  constexpr operator K() const { return K{}; }

  // But we can assign to guys who are identical!

  template<typename K> requires(std::derived_from<K, node<T>>)
  constexpr operator K() const { return K{}; }

};

// Value Type

template<typename T>
concept value_type =
    std::is_same_v<T,bool>
||  std::is_same_v<T,char>
||  std::is_same_v<T,int>
||  std::is_same_v<T,long>
||  std::is_same_v<T,size_t>
||  std::is_same_v<T,float>
||  std::is_same_v<T,double>;

template<constexpr_string T, value_type K>
constexpr K val(const K k){
    std::cout<<T.value<<std::endl;
    return std::move(k);
}

static void done(){
  std::cout<<"DONE"<<std::endl;
}

// Print the Abstract Syntax Tree

static void ast(){
  std::cout<<"PRINTING AST:"<<std::endl;
}

}

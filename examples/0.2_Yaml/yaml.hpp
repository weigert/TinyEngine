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


template<constexpr_string S>
struct key {
  static constexpr char const* _key = S;
  // we can now do compile time assertion that the key is actually valid!
};

template<constexpr_string S>
std::ostream& operator<<(std::ostream& os, const key<S>& k){
    return os << S;
}

/*
================================================================================
                                Yaml Nodes
================================================================================
*/

// Nodes

struct node{
  virtual void print(std::ostream& os){
    os << "NOTHING";
  }
};

std::ostream& operator<<(std::ostream& os, node& n){
    n.print(os);
    return os;
}

// Node-Type: Value

struct val_base: node{};

template<typename T>
concept yaml_val = std::derived_from<T, val_base>;

template<typename V>
concept value_type =
    std::is_same_v<V, bool>
||  std::is_same_v<V, char>
||  std::is_same_v<V, int>
||  std::is_same_v<V, long>
||  std::is_same_v<V, size_t>
||  std::is_same_v<V, float>
||  std::is_same_v<V, double>;

template<value_type V>
struct val: val_base {

  V value;

  val(const V& v):value{v}{}

  constexpr operator V() const { return value; }

  void print(std::ostream& os) override {
    os << value;
  }

};

// Node-Type: Object

struct obj_base: node {};

template<typename T>
concept yaml_obj = std::derived_from<T, obj_base>;

//template<constexpr_string Key>
struct obj: obj_base {

  std::map<const char*, node*> sub;

  // Sub-Node Constructors

  template<constexpr_string Key, value_type V>
  constexpr yaml::val<V> _val(const V& v){
    yaml::val<V>* s = new yaml::val<V>(v);
    sub[Key.value] = s;
    return *s;
  }

  // Moved: Pointer remains Valid!

  template<constexpr_string Key, yaml_obj V>
  constexpr V _obj(const V& _v){
    V* v = new V();
    sub[Key.value] = v;
    return *v;
  }

  void print(std::ostream& os){
    os << std::endl;
    for(auto& s: sub){
      os << s.first << ": ";
      s.second->print(os);
      os << std::endl;
    }
  }

};

// Print the Abstract Syntax Tree

static void ast(){
  std::cout<<"PRINTING AST:"<<std::endl;
}

}

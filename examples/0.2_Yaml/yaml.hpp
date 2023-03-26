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

/*

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

template<constexpr_string Key>
struct new_key: key_base {
  key<Key> _key;
};

// Specific, Distinct Types of Keys!

template<constexpr_string Key>
struct val_key: new_key<Key> {};

template<constexpr_string Key>
struct obj_key: new_key<Key> {};

// Specific Types derived from specific keys


template<is_key key>
struct new_node {};


template<constexpr_string Key>
struct new_node<val_key<Key>> {
  static constexpr const char* type = "val";
};

template<constexpr_string Key>
using val_node = new_node<val_key<Key>>;

template<constexpr_string Key>
struct new_node<obj_key<Key>> {
  static constexpr const char* type = "obj";
};

template<constexpr_string Key>
using obj_node = new_node<obj_key<Key>>;

// Note: Statically Assert if Two Template Arguments are the same!
//  Not allowed!

//template<constexpr_string... Keys>
//struct unique_key_set{
//  static constexpr bool value = is_unique<new_key<Keys>...>::value;
//};












struct obj_base{};

template<typename T>
concept is_obj = std::derived_from<T, obj_base>;

template<is_key... Keys>
struct new_obj: obj_base {

  //static_assert(unique_key_set<Keys...>::value)

  std::tuple<new_node<Keys>...> nodes;

  template<is_key Key> struct index {
    static constexpr size_t value = tuple_index<Key, 0, Keys...>::value;
  };

  template<is_key Key>
  new_node<Key>& get() {
    return std::get<index<Key>::value>(nodes);
  }

//  new_obj(){
//    std::apply([](auto&&... args) {((std::cout << args.type << '\n'), ...);}, nodes);
//  }

  // Assign a value!
  template<constexpr_string Key, typename V>
  V val(const V& v){
    static_assert(value_type<V>, "type is not a value type");
    static_assert(is_contained<val_key<Key>, Keys...>::value, "key for yaml::val does not exist in yaml::obj");
//    std::cout<<"INDEX: "<<index<val_key<Key>>::value<<std::endl;
    val_node<Key> node = get<val_key<Key>>();
    std::cout<<"INSERTED: "<<node.type<<std::endl;
//    new_node<val_key<Key>> t = std::get<>(nodes);
    return std::move(v);
  }

  template<constexpr_string Key, typename V>
  // requires()
  V obj(const V& v){
    static_assert(is_obj<V>, "type is not a derived type of yaml::obj");
    static_assert(is_contained<obj_key<Key>, Keys...>::value, "key for yaml::obj does not exist in yaml::obj");
//    std::cout<<"INDEX: "<<index<val_key<Key>>::value<<std::endl;
//    new_node<val_key<Key>> node = get<val_key<Key>>();
//    new_node<val_key<Key>> t = std::get<>(nodes);

    obj_node<Key> node = get<obj_key<Key>>();
    std::cout<<"INSERTED: "<<node.type<<std::endl;

    return std::move(v);
  }

};

// Print the Abstract Syntax Tree

static void ast(){
  std::cout<<"PRINTING AST:"<<std::endl;
}

}

#ifndef TINYENGINE_YAML
#define TINYENGINE_YAML

/*
===================================================
<TinyEngine/yaml>
Author: Nicholas McDonald (2022)

A small .yaml parser with direct loading into user
defined structs. Inspired by golang semantic tags.
Utilizes pointer-magic to avoid an indexing struct.
===================================================

Features:
- Basic Types (char, int, float, double, string)
- Struct Types (any user-defined, tagged struct)
- STL Containers (Templated w. Basic Type)
- Nestings of the above

ToDo:
- More Atomic Types
- Only throw a warning without failure?
- STL Container of Struct

- Broken: STL Container of Struct?
- Broken: Memory Ambiguous Struct Layouts
    The nesting is messed up!!!

Last ditch effort:
    instead of using the set,
    somehow store the variable / object itself
    and see if you can use is_member_pointer to sort the order.
    This requires doing some weird stuff though.
    If that works, then it would be great.

It is not possible to take any variable and see if it is a member pointer of another type.
Because it always takes the identifier as literal, which must therefore be known.
Therefore, macros would be necessary which I don't want.

There is no way of solving the struct nesting problem.
*/

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <map>
#include <type_traits>
#include "../external/mini-yaml.h"

namespace yaml {
using namespace std;

// Constants

bool OMITEMPTY = true;

// Formatted YAML Exception

class exception : public std::exception {
public:

    string msg;

    exception(){}
    exception(string f):msg{f}{}
    exception(const char* f):msg{string(f)}{}

    template <typename... Types>
    exception(const char* f, Types... r){
        msg = format(f, r...);
    }

    template<typename... Types>
    string format(const char* f, Types... t){
        static char tmp[4096];
        size_t n = sprintf(tmp, f, t...);
        return string(tmp);
    }

    const char* what(){
        return msg.c_str();
    }

};

// YAML Type Print Helper

const char* type(Yaml::Node::eType t){
    if(t == Yaml::Node::eType::None)            return "none";
    if(t == Yaml::Node::eType::ScalarType)      return "scalar";
    if(t == Yaml::Node::eType::SequenceType)    return "sequence";
    if(t == Yaml::Node::eType::MapType)         return "map";
    return "unknown";
}

/*
===================================================
            Global Indexing Structure
===================================================
*/

typedef std::pair<const string&, Yaml::Node&> pair;

struct ind {
    void* p;                            // Pointer to Memory
    const size_t s;                     // sizeof(value)
    const char (*t);                    // typeof(value)
    const char (*n);                    // Name of Key
    void (*f)(const yaml::ind&, pair);  // Parse Function Pointer
    const int o = 0;                    // Nesting Level
};

ostream& operator<<(ostream& os, const ind& i){
    os << "\"" << i.n << "\" (";
    os << i.p << ", ";
    os << i.s << ", ";
    os << i.o << ", ";
    os << i.t << ")";
    return os;
}

// Index Struct and Set

struct ind_sort {
    bool operator() (ind a, ind b) const {
        if(a.p < b.p) return true;  // Memory Position
        if(a.p > b.p) return false;
        if(a.s > b.s) return true;  // Variable / Object Size
        if(a.s < b.s) return false;
        if(a.o > b.o) return true;  // Nesting Level
        if(a.o < b.o) return false;
        return false;
    }
};

typedef set<ind, ind_sort> indset;
indset index;

ostream& operator<<(ostream& os, const indset::iterator& i){
    if(i == index.end())
        return (os << ind{NULL, 0, "NULL", "NO_KEY"});
    return (os << (*i));
}

// Search Functions

template<typename T>
const char* key(T* t){
    for(auto& i: index)
    if(i.p == (void*)t)
        return i.n;
    return "";
}

template<typename T>
indset::iterator find(T& t){
    indset::iterator it = index.begin();
    for(; it != index.end(); it++)
    if((*it).p == (void*)&t && (*it).s == sizeof(t))
        break;
    return it;
}

indset::iterator find(const yaml::ind& i){
    indset::iterator it = index.begin();
    for(; it != index.end(); it++)
    if((*it).p == (void*)(i.p) && (*it).s == i.s && (*it).o == i.o)
        break;
    return it;
}

indset sameall(const yaml::ind& start){

    indset sameindex;

    indset::iterator it = find(start);

    while((it) != index.end() && start.p == it->p)
        sameindex.insert((*it++));       // Add New Sub-Node  

    return sameindex;

}

indset suball(const yaml::ind& start){

    indset subindex;

    indset::iterator it = find(start);
    if(it == index.end())
        return subindex;

    uint8_t* seek_end = (uint8_t*)(start.p) + start.s;
    while((++it) != index.end() && (uint8_t*)it->p < seek_end)
        subindex.insert((*it));       // Add New Sub-Node  

    return subindex;

}

indset sub(const yaml::ind& start){

    indset subindex;

    subindex = suball(start);
    if(subindex.empty())
        return subindex;

    // Remove Redundant Sub-Elements

    indset::iterator it = find(start);
    if(it == index.end())
        return subindex;

    uint8_t* seek_end = (uint8_t*)(start.p) + start.s;
    while((++it) != index.end() && (uint8_t*)(it->p) < seek_end){
        indset nsubindex = suball((*it));
        for(auto& ns: nsubindex)
            subindex.erase(ns);
    }

    return subindex;

}

/*
===================================================
        Templated In-Place Yaml-Node Parsing
===================================================
*/

// Templated Base-Type In-Place Constructors

template<typename T>
inline void parse(const yaml::ind& p, string s){
    throw exception( "no parser for type (\"%s\")", typeid(T).name() );
}

template<>
inline void parse<char>(const yaml::ind& p, string s){
    try{ new ((char*)(p.p)) char(s.c_str()[0]); }
    catch(...) {
        throw exception( "not char (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<int>(const yaml::ind& p, string s){
    try{ new ((int*)(p.p)) int(std::stoi(s)); }
    catch(...) { 
        throw exception( "not integer (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<float>(const yaml::ind& p, string s){
    try { new ((float*)(p.p)) float(std::stof(s)); }
    catch(...){
        throw exception( "not float (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<double>(const yaml::ind& p, string s){
    try { new ((double*)(p.p)) double(std::stod(s)); }
    catch(...){
        throw exception( "not double (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<string>(const yaml::ind& p, string s){
    try { new ((string*)(p.p)) string(s); }
    catch(...){
        throw exception( "not string (\"%s\")", s.c_str() );
    }
}

// Templated Pair-Parse Functions

template<typename T>
constexpr inline void (*parser( T ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( glm::tvec2<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( glm::tvec3<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( glm::tvec4<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( glm::tmat2x2<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( glm::tmat3x3<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( glm::tmat4x4<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( vector<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( set<T> ))(const yaml::ind&, pair);

template<typename Tkey, typename Tval>
constexpr inline void (*parser( map<Tkey, Tval> ))(const yaml::ind&, pair);

// Hierarchical Parse Functions for Complex Types

// Atomic Type

template<typename T>
inline void parse_atomic(const yaml::ind& p, pair s){

    indset subindex = sub(p);   // Search Memory Subindex

    if(subindex.empty()){       // Non-Structy: Require Scalar

        if(!s.second.IsScalar())
            throw exception("have (%s), want (scalar)", type(s.second.Type()));

        parse<T>(p, s.second.As<string>());
        return;

    }

    if(s.second.IsScalar())
        throw exception("have (%s), want (!scalar)", type(s.second.Type()));

    auto it = s.second.Begin();
    for(auto& sub: subindex){

        indset::iterator start = find(sub);

        if(it == s.second.End()){
            cout<<"NOOOOO"<<endl;
            throw exception("can't find node for key (%s)", (*start).n);
        }

        try {
            (*start).f((*start), (*it));
        } catch( exception& e ){
            throw exception("can't parse variable (%s), %s", (*start).n, e.what());
        }

        it++;

    }

    if(it != s.second.End())
        throw exception("can't parse variable (%s), too many elements", p.n);

}

// c++ array

template<typename T, size_t N>
inline void parse_array(const yaml::ind& p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    int n = 0;
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        if(N <= n)
            throw exception("too many array elements");

        T t;
        yaml::ind subind = {
            (T*)(p.p) + n,
            sizeof(T),
            typeid(T).name(),
            "KEYYY????",
            parser(t)
        };

        try {
            parser(t)(subind, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node (%d). %s", n, e.what());
        }

        n++;

    }

    if(n < N)
        throw exception("too few array elements");

}

// std::vector parser

template<typename T>
inline void parse_vector(const yaml::ind& p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    vector<T>* vt = (vector<T>*)(p.p);
    
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        T t;
        yaml::ind subind = {
            &t,
            sizeof(T),
            typeid(T).name(),
            "KEYYY????",
            parser(t)
        };

        try {
            parser(t)(subind, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node. %s", e.what());
        }
        vt->push_back(t);

    }

}

// std::set parser

template<typename T>
inline void parse_set(const yaml::ind& p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    set<T>* st = (set<T>*)(p.p);
    
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        T t;
        yaml::ind subind = {
            &t,
            sizeof(T),
            typeid(T).name(),
            "KEYYY????",
            parser(t)
        };

        try {
            parser(t)(subind, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node. %s", e.what());
        }
        st->insert(t);

    }

}

// std::map type

template<typename Tkey, typename Tval>
inline void parse_map(const yaml::ind& p, pair s){

    if(!s.second.IsMap())
        throw exception("have (%s), want (map)", type(s.second.Type()));

    map<Tkey, Tval>* mkv = (map<Tkey, Tval>*)(p.p);
    
    for(auto it = s.second.Begin(); it != s.second.End(); it++){
        
        Tkey key;
        yaml::ind subind_key = {
            &key,
            sizeof(key),
            typeid(Tkey).name(),
            "KEYYY????",
            parser(key)
        };

        Tval val;
        yaml::ind subind_val = {
            &val,
            sizeof(val),
            typeid(Tval).name(),
            "VAL????",
            parser(val)
        };

        try {
            parse<Tkey>(subind_key, (*it).first);
        }
        catch( exception& e ){
            throw exception("can't parse sub-node key. %s", e.what());
        }

        try {
            parser(Tval())(subind_val, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node value. %s", e.what());
        }

        (*mkv)[key] = val;

    }

}

// Templated Pair-Parse Function Pointer Retriever

template<typename T>
constexpr inline void (*parser( T ))(const yaml::ind&, pair){
    return &parse_atomic<T>;
}

template<typename T>
constexpr inline void (*parser( glm::tvec2<T> ))(const yaml::ind&, pair){
    return &parse_array<T, 2>;
}

template<typename T>
constexpr inline void (*parser( glm::tvec3<T> ))(const yaml::ind&, pair){
    return &parse_array<T, 3>;
}

template<typename T>
constexpr inline void (*parser( glm::tvec4<T> ))(const yaml::ind&, pair){
    return &parse_array<T, 4>;
}

template<typename T>
constexpr inline void (*parser( glm::tmat2x2<T> ))(const yaml::ind&, pair){
    return &parse_array<glm::tvec2<T>, 2>;
}

template<typename T>
constexpr inline void (*parser( glm::tmat3x3<T> ))(const yaml::ind&, pair){
    return &parse_array<glm::tvec3<T>, 3>;
}

template<typename T>
constexpr inline void (*parser( glm::tmat4x4<T> ))(const yaml::ind&, pair){
    return &parse_array<glm::tvec4<T>, 4>;
}

template<typename T>
constexpr inline void (*parser(vector<T>))(const yaml::ind&, pair){
    return &parse_vector<T>;
}

template<typename T>
constexpr inline void (*parser(set<T>))(const yaml::ind&, pair){
    return &parse_set<T>;
}

template<typename Tkey, typename Tval>
constexpr inline void (*parser(map<Tkey, Tval>))(const yaml::ind&, pair){
    return &parse_map<Tkey, Tval>;
}

/*
===================================================
        Raw Memory Index / Key Assignment
===================================================
*/

// Index Construction Helper

struct ind_key {
    const char* key;
};

constexpr ind_key key(const char* key){
    return ind_key{key};
}

// Scalar

template<typename T>
constexpr inline void insert(T& var, const yaml::ind_key& n, int order = 0){

    indset::iterator i = find(var);
    indset subindex = sameall((*i));   // Search Memory Subindex

    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        n.key,
        parser(var),
        order
    });

}

template <class T> // concept
concept is_class = std::is_class<T>::value;

template<is_class T>
constexpr inline void insert(T& var, const yaml::ind_key& n, int order = 0){

    indset::iterator i = find(var);
    indset subindex = sameall((*i));   // Search Memory Subindex

    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        n.key,
        parser(var),
        order
    });

}

template <class T> // concept
concept is_empty = std::is_empty<T>::value;

template<is_empty T>
constexpr inline void insert(T& var, const yaml::ind_key& n, int order = 0){
    cout<<"CANT ASSIGN TO EMPTY STRUCT"<<endl;
}

// Array

template<typename T, size_t N>
constexpr inline void insert(T(&var)[N], const yaml::ind_key& k, int order = 0){

    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        k.key,
        parser(var),
        order
    });

    for(size_t n = 0; n < N; n++)
        insert(var[n], key(std::to_string(n).c_str()), order - 1);

}

template<typename T>
T& operator<<(T& var, const yaml::ind_key& n){
    insert(var, n);
    return var;
}

/*
===================================================
        Recursive YAML Loading Functions
===================================================
*/

// Load into Struct

void extract(indset::iterator& start, pair node){

    // Dereference Start Node, Get End-Point

    try {
        (*start).f((*start), node); 
    } catch( exception& e ){
        throw exception("can't parse variable (%s), %s", (*start).n, e.what());
    }

}

template<typename T>
bool load(T* var, const char* filename){

    Yaml::Node root;

    try{
        Yaml::Parse(root, filename); 
    } catch(const Yaml::Exception e){
        printf("YAML Parse Error: (%d) %s\n", e.Type(), e.what());
        return false;
    }

    if(root.Size() == 0){        
        printf("YAML Parse Error: empty file (%s)\n", filename);
        return false;
    }

    indset::iterator start = yaml::find(*var);
    if(start == index.end()){
        printf("YAML Extract Error: no key assigned (%p)\n", var);
        return false;
    }

    // Find the YAML Node

    auto it = root.Begin();

    try {

        while(it != root.End() && (*it).first != (*start).n)
            it++;
           
        if(it == root.End())
            throw exception("can't find node for key (%s)", (*start).n);

    } catch( exception& e ){
        printf("YAML Extract Error: %s\n", e.what());
        return false;
    }
    
    try {
        extract(start, (*it));
    } catch( exception& e ){
        printf("YAML Extract Error: %s\n", e.what());
        return false;
    }

    return true;

}

};

#endif
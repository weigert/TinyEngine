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

ToDo:
- STL Containers (Map, Set, Array)
- Vector of Struct
- More Atomic Types

*/

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <map>
#include "../external/mini-yaml.h"

namespace yaml {
using namespace std;

// Constants

bool OMITEMPTY = true;

// Formatted YAML Exception

class exception : public std::exception {
public:

    string msg;

    // Basic Constructor

    exception(){}
    exception(string f):msg{f}{}
    exception(const char* f):msg{string(f)}{}

    // Variadic Formatted Constructor

    template <typename... Types>
    exception(const char* f, Types... r){
        msg = format(f, r...);
    }

    template<typename... Types>
    string format(const char* f, Types... t){

        char* tmp = new char[2*strlen(f)];
        size_t n = sprintf(tmp, f, t...);

        string s(tmp);
        delete[] tmp;
        return s;

    }

    const char* what(){
        return msg.c_str();
    }

};

const char* type(Yaml::Node::eType t){
    if(t == Yaml::Node::eType::None)            return "none";
    if(t == Yaml::Node::eType::ScalarType)      return "scalar";
    if(t == Yaml::Node::eType::SequenceType)    return "sequence";
    if(t == Yaml::Node::eType::MapType)         return "map";
    return "unknown";
}

/*
===================================================
        Templated In-Place Yaml-Node Parsing
===================================================
*/

// In-Place Atomic String-Parse Functions

inline void parse_char(const void* p, string s){
    *((char*)p) = s.c_str()[0];
}

inline void parse_int(const void* p, string s){
    try { *((int*)p) = std::stoi(s); } 
    catch(const invalid_argument& e){
        throw exception( "not integer (\"%s\")", s.c_str() );
    }
}

inline void parse_float(const void* p, string s){
    try { *((float*)p) = std::stof(s); } 
    catch(const invalid_argument& e){
        throw exception( "not float (\"%s\")", s.c_str() );
    }
}

inline void parse_double(const void* p, string s){
    try { *((double*)p) = std::stod(s); } 
    catch(const invalid_argument& e){
        throw exception( "not double (\"%s\")", s.c_str() );
    }
}

inline void parse_string(const void* p, string s){
    *((string*)p) = s;
}

// Templated Atomic String-Parse Function Pointer Retriever

template<typename T>
constexpr void (*parse_atomic(T))(const void*, string){
    return NULL;
}

template<>
constexpr void (*parse_atomic(char))(const void*, string){
    return &parse_char;
}

template<>
constexpr void (*parse_atomic(int))(const void*, string){
    return &parse_int;
}

template<>
constexpr void (*parse_atomic(float))(const void*, string){
    return &parse_float;
}

template<>
constexpr void (*parse_atomic(double))(const void*, string){
    return &parse_double;
}

template<>
void (*parse_atomic(string))(const void*, string){
    return &parse_string;
}

// Templated Pair-Parse Functions

typedef std::pair<const string&, Yaml::Node&> pair;

// Pass-Through Default Function: Interpret Node Value
//      ...and retriever!

template<typename T>
constexpr void (*parser(T))(const void*, pair);

template<typename T>
constexpr void parser_atomic(const void* p, pair t){

    if(!t.second.IsScalar())
        throw exception("have (%s), want (scalar)", type(t.second.Type()));

    if(parse_atomic(T()) == NULL)
        throw exception( "no parser for type (\"%s\")", typeid(T()).name() );

    parse_atomic(T())(p, t.second.As<string>());

}

// c++ array

template<typename T, size_t N>
inline void parse_array(const void* p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    if(parser(T()) == NULL)
        throw exception( "no parser for type (\"%s\")", typeid(T()).name() );

    int n = 0;
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        if(!(*it).second.IsScalar())
            throw exception("invalid sub-node. have (%s), want (scalar)", type((*it).second.Type()));
        
        try {
            parser(T())((T*)p + n, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node. %s", e.what());
        }

        n++;
        if(N < n)
            throw exception("too many array elements");

    }

}

// std::vector parser

template<typename T>
inline void parse_vector(const void* p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    T t;
    if(parser(t) == NULL)
        throw exception( "no parser for type (\"%s\")", typeid(t).name() );

    vector<T>* vt = (vector<T>*)p;
    for(auto it = s.second.Begin(); it != s.second.End(); it++){
        
        if(!(*it).second.IsScalar())
            throw exception("invalid sub-node. have (%s), want (scalar)", type((*it).second.Type()));
        
        try {
            parser(t)(&t, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node. %s", e.what());
        }
        vt->push_back(t);

    }

}

// std::set parser

template<typename T>
inline void parse_set(const void* p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    T t;
    if(parser(t) == NULL)
        throw exception( "no parser for type (\"%s\")", typeid(t).name() );

    set<T>* st = (set<T>*)p;
    for(auto it = s.second.Begin(); it != s.second.End(); it++){
        
        if(!(*it).second.IsScalar())
            throw exception("invalid sub-node. have (%s), want (scalar)", type((*it).second.Type()));
        
        try {
            parser(t)(&t, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node. %s", e.what());
        }
        st->insert(t);

    }

}

// std::map type

template<typename Tkey, typename Tval>
inline void parse_map(const void* p, pair s){

    if(!s.second.IsMap())
        throw exception("have (%s), want (map)", type(s.second.Type()));

    Tkey key;
    if(parser(key) == NULL)
        throw exception( "no parser for type (\"%s\")", typeid(key).name() );

    Tval val;
    if(parser(val) == NULL)
        throw exception( "no parser for type (\"%s\")", typeid(val).name() );

    map<Tkey, Tval>* mkv = (map<Tkey, Tval>*)p;
    for(auto it = s.second.Begin(); it != s.second.End(); it++){
        
        if(!(*it).second.IsScalar())
            throw exception("invalid sub-node. have (%s), want (scalar)", type((*it).second.Type()));
        
        try {
            parse_atomic(key)(&key, (*it).first);
        }
        catch( exception& e ){
            throw exception("can't parse sub-node key. %s", e.what());
        }

        try {
            parse_atomic(val)(&val, (*it).second.As<string>());
        }
        catch( exception& e ){
            throw exception("can't parse sub-node value. %s", e.what());
        }

        (*mkv)[key] = val;

    }

}

// Templated Pair-Parse Function Pointer Retriever

template<typename T>
constexpr void (*parser(T))(const void*, pair){
    if(parse_atomic(T()) == NULL) return NULL;
    return &parser_atomic<T>;
}

template<typename T, size_t N>
constexpr void (*parser( T(&t)[N] ))(const void*, pair){
    return &parse_array<T, N>;
}

template<typename T>
constexpr void (*parser(vector<T>))(const void*, pair){
    return &parse_vector<T>;
}

template<typename T>
constexpr void (*parser(set<T>))(const void*, pair){
    return &parse_set<T>;
}

template<typename Tkey, typename Tval>
constexpr void (*parser(map<Tkey, Tval>))(const void*, pair){
    return &parse_map<Tkey, Tval>;
}

/*
===================================================
        Raw Memory Index / Key Assignment
===================================================
*/

// Index Struct and Set

struct ind {
    void* p;                            // Pointer to Memory
    const size_t s;                     // sizeof(value)
    const char (*t);                    // typeof(value)
    const char (*n);                    // Name of Key
    void (*f)(const void*, pair);       // Parse Function Pointer
};

ostream& operator<<(ostream& os, const ind& i){
    os << "\"" << i.n << "\" (";
    os << i.p << ", ";
    os << i.s << ", ";
    os << i.t << ")";
    return os; 
}

struct ind_sort {
    bool operator() (ind a, ind b) const {
        if(a.p < b.p) return true;
        if(a.p > b.p) return false;
        if(a.s > b.s) return true;
        if(a.s < b.s) return false;
        return false;
    }
};

typedef set<ind, ind_sort> indset;
indset index;

template<typename T>
const char* key(T* t){
    for(auto& i: index)
    if(i.p == (void*)t)
        return i.n;
    return "";
}

template<typename T>
indset::iterator find(T* t){
    indset::iterator it = index.begin();
    for(; it != index.end(); it++)
    if((*it).p == (void*)t)
        break;
    return it;
}

ostream& operator<<(ostream& os, const indset::iterator& i){
    if(i == index.end())
        return (os << ind{NULL, 0, "NULL", "NO_KEY"});
    return (os << (*i));
}

// Index Construction Helper

struct ind_key {
    const char* key;
};

ind_key key(const char* key){
    return ind_key{key};
}

template<typename T>
T& operator<<(T& var, const yaml::ind_key& n){
    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        n.key,
        parser(var)
    });
    return var;
}

// Load into Struct

void extract(indset& subindex, indset::iterator& start, Yaml::Node& node){

    // Dereference Start Node, Get End-Point

    yaml::ind start_v = *start;

    // Find the YAML Node

    auto it = node.Begin();
    while(it != node.End() && (*it).first != start_v.n)
        it++;
       
    if(it == node.End())
        throw exception("can't find node for key (%s)", start_v.n);

    // Search for Sub-Nodes

    indset nsubindex;
    uint8_t* seek_end = (uint8_t*)(start_v.p) + start_v.s;
    while((uint8_t*)(start->p) < seek_end){

        if((++start) == index.end())        // Get Next Element
            break;

        if(start->p < seek_end){            // Element Fits Inside
            nsubindex.insert(*start);       // Add New Sub-Node
            subindex.erase(*start);         // Remove Node from Parent
        }

    }

    // Direct Extraction

    if(nsubindex.empty()){

        // Non-Parsable Variable

        if(start_v.f == NULL)
            throw exception("can't parse variable (%s), type (%s) has no parser", start_v.n, start_v.t);

        try {
            start_v.f(start_v.p, (*it)); 
        } catch( exception& e ){
            throw exception("can't parse variable (%s), %s", start_v.n, e.what());
        }
    
    }

    // Sub-Index Recursion

    else if((*it).second.IsMap()){

        for(auto& sub: nsubindex){

            for(start = index.begin(); start != index.end(); start++)
            if((*start).p == sub.p && (*start).s == sub.s)
                break;

            try {
                extract(nsubindex, start, (*it).second);
            } catch( exception& e ){
                printf("YAML Extract Error: %s\n", e.what());
            }

        }

    }

    // Non-Map Type Node

    else throw exception("invalid node for key (%s). have (%s), want (map)", start_v.n, type((*it).second.Type()));

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

    indset::iterator start = yaml::find(var);
    if(start == index.end()){
        printf("YAML Extract Error: no key assigned (%p)\n", var);
        return false;
    }

    indset nindex = index;
    
    try {
        extract(nindex, start, root);
    } catch( exception& e ){
        printf("YAML Extract Error: %s\n", e.what());
        return false;
    }

    return true;
}

};

#endif
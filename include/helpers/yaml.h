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
- Basic Types
    (char, int, float, double, string)
- Struct Types
    (any user-defined, tagged struct)
- STL Containers (Templated w. Basic Type)
    (Vector, Set, Map)
- Nestings of STL Containers and Structs
    - Struct of Struct (of Struct...)
    - Struct of STL Containers (Arbitrary)
    - 

ToDo:
- More Atomic Types
- Only throw a warning without failure?
- STL Container of Struct

- Broken: Array of Struct
    Broken because the beginning of the struct has no key,
    Making it non-extractable.
    When I register an array of structs, I have to register
    all structs as sub-nodes as well.
- Broken: STL Container of Struct

- Direct, Non-Struct Arrays are also not broken.
- This is because they only register a single key and
 use their atomic parser.
- BROKEN: BASIC STRUCT BUT MIS-DEFINED IN CONFIG.YAML AS ARRAY!
    CURRENTLY, THERE IS NO MECHANISM FOR RECOGNIZING THAT WE WANT A MAP!!!
Note that for vectors, this works fine because it uses its atomic parser.
Note that the same is valid for simple arrays, because it can check that it wants a sequence.

THEREFORE, IN ORDER TO FIX STRUCTS, THEY HAVE TO CALL TYPE CHECKING ATOMIC FUNCTIONS!!!!!!!!!

Something like:
- Array type variable calls its atomic parser, which will then call atomic parser for a struct-type,
which will then correctly parse itself, potentially calling array type, and so on.

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

        static char tmp[4096];
        size_t n = sprintf(tmp, f, t...);
        return string(tmp);

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

// In-Place Atomic String->Template Parse Functions

template<typename T>
inline void parse(const void* p, string s){
    throw exception( "no parser for type (\"%s\")", typeid(T).name() );
}

template<>
inline void parse<char>(const void* p, string s){
    try{ new ((char*)p) char(s.c_str()[0]); }
    catch(...) { 
        throw exception( "not char (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<int>(const void* p, string s){
    try{ new ((int*)p) int(std::stoi(s)); }
    catch(...) { 
        throw exception( "not integer (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<float>(const void* p, string s){
    try { new ((float*)p) float(std::stof(s)); } 
    catch(const invalid_argument& e){
        throw exception( "not float (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<double>(const void* p, string s){
    try { new ((double*)p) double(std::stod(s)); } 
    catch(const invalid_argument& e){
        throw exception( "not double (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<string>(const void* p, string s){
    try { new ((string*)p) string(s); } 
    catch(const invalid_argument& e){
        throw exception( "not double (\"%s\")", s.c_str() );
    }
}

// Templated Pair-Parse Functions

typedef std::pair<const string&, Yaml::Node&> pair;

// Pass-Through Default Function: Interpret Node Value
//      ...and retriever!

template<typename T>
constexpr inline void (*parser( T ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( glm::tvec2<T> ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( glm::tvec3<T> ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( glm::tvec4<T> ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( glm::tmat2x2<T> ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( glm::tmat3x3<T> ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( glm::tmat4x4<T> ))(const void*, pair);

template<typename T, size_t N>
constexpr inline void (*parser( T(&t)[N] ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( vector<T> ))(const void*, pair);

template<typename T>
constexpr inline void (*parser( set<T> ))(const void*, pair);

template<typename Tkey, typename Tval>
constexpr inline void (*parser( map<Tkey, Tval> ))(const void*, pair);

// Implementations

 /*

    // Search for Sub-Nodes


    // Direct Extraction

    if(subindex.empty()){

        
    
    }

    if(start_v.f == &parse_array<float, 5>)
        cout<<"IS ATOMIC"<<endl;

    // Sub-Index Recursion: Struct

    else if(node.second.IsMap()){

        for(auto& sub: subindex){

            start = find(sub);

            auto it = node.second.Begin();
            while(it != node.second.End() && (*it).first != (*start).n)
                it++;

            if(it == node.second.End())
                throw exception("can't find node for key (%s)", (*start).n);

            try {
                extract(start, (*it));
            } catch( exception& e ){
                throw exception("can't parse variable (%s), %s", start_v.n, e.what());
            }

        }

    }

    // Sub-Index Recursion: Array of Struct

    else if(node.second.IsSequence()){

        cout<<"ARRAY OF STRUCT"<<endl;

        auto it = node.second.Begin();

        for(auto& sub: subindex){

            start = find(sub);

        //    cout<<(*start).n<<endl;

            auto itt = (*it).second.Begin();

            while(itt != (*it).second.End() && (*itt).first != (*start).n)
                itt++;

            try {
                extract(start, (*itt));
            } catch( exception& e ){   
                throw exception("can't parse variable (%s), %s", start_v.n, e.what());
            }

            itt++;
            if(itt == (*it).second.End()){
                it++;
            }


        }

    }

    // Non-Map Type Node

    else throw exception("invalid node for key (%s). have (%s), want (!none)", start_v.n, type(node.second.Type()));

    */

template<typename T>
inline void parse_atomic(const void* p, pair s){


    /*
        Here I need to make sure that
        I have something which actually IS a scalar.
    */

    //    indset subindex = sub(start_v);

    // The Map is Scalar: 

    if(s.second.IsScalar()){
        parse<T>(p, s.second.As<string>());
        return;
    }

        /*
        The question is:
        - Do we have a struct?
        - An array of structs (this initializes a subindex)
        - A struct of structs?
    
        Then I can effectively decide what to do with nodes.
        But note that I actually need access to the ind for this to work...
        What is stopping this?

    */


    cout<<"ATTEMPTING ATOMIC PARSE ON"<<endl;
    T t;
    cout<<typeid(t).name()<<endl;


    throw exception("have (%s), want (scalar)", type(s.second.Type()));

}

// c++ array

template<typename T, size_t N>
inline void parse_array(const void* p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    int n = 0;
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        if(N <= n)
            throw exception("too many array elements");

        T t;
        try {
            parser(t)((T*)p + n, (*it));
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
inline void parse_vector(const void* p, pair s){

    if(!s.second.IsSequence())
        throw exception("have (%s), want (sequence)", type(s.second.Type()));

    vector<T>* vt = (vector<T>*)p;
    
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        T t;

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

    set<T>* st = (set<T>*)p;
    
    for(auto it = s.second.Begin(); it != s.second.End(); it++){

        T t;

        try {
            parser(T())(&t, (*it));
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

    map<Tkey, Tval>* mkv = (map<Tkey, Tval>*)p;
    
    for(auto it = s.second.Begin(); it != s.second.End(); it++){
        
        Tkey key;
        Tval val;

        try {
            parse<Tkey>(&key, (*it).first);
        }
        catch( exception& e ){
            throw exception("can't parse sub-node key. %s", e.what());
        }

        try {
            parser(Tval())(&val, (*it));
        }
        catch( exception& e ){
            throw exception("can't parse sub-node value. %s", e.what());
        }

        (*mkv)[key] = val;

    }

}

// Templated Pair-Parse Function Pointer Retriever

template<typename T>
constexpr inline void (*parser( T ))(const void*, pair){
    return &parse_atomic<T>;
}

template<typename T, size_t N>
constexpr inline void (*parser( T(&t)[N] ))(const void*, pair){
    return &parse_array<T, N>;
}

template<typename T>
constexpr inline void (*parser( glm::tvec2<T> ))(const void*, pair){
    return &parse_array<T, 2>;
}

template<typename T>
constexpr inline void (*parser( glm::tvec3<T> ))(const void*, pair){
    return &parse_array<T, 3>;
}

template<typename T>
constexpr inline void (*parser( glm::tvec4<T> ))(const void*, pair){
    return &parse_array<T, 4>;
}

template<typename T>
constexpr inline void (*parser( glm::tmat2x2<T> ))(const void*, pair){
    return &parse_array<glm::tvec2<T>, 2>;
}

template<typename T>
constexpr inline void (*parser( glm::tmat3x3<T> ))(const void*, pair){
    return &parse_array<glm::tvec3<T>, 3>;
}

template<typename T>
constexpr inline void (*parser( glm::tmat4x4<T> ))(const void*, pair){
    return &parse_array<glm::tvec4<T>, 4>;
}

template<typename T>
constexpr inline void (*parser(vector<T>))(const void*, pair){
    return &parse_vector<T>;
}

template<typename T>
constexpr inline void (*parser(set<T>))(const void*, pair){
    return &parse_set<T>;
}

template<typename Tkey, typename Tval>
constexpr inline void (*parser(map<Tkey, Tval>))(const void*, pair){
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
    if((*it).p == (void*)t && (*it).s == sizeof(T))
        break;
    return it;
}

indset::iterator find(const yaml::ind& i){
    indset::iterator it = index.begin();
    for(; it != index.end(); it++)
    if((*it).p == (void*)(i.p) && (*it).s == i.s)
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

constexpr ind_key key(const char* key){
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

/*
===================================================
        Recursive YAML Loading Functions
===================================================
*/

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

    //cout<<start.n<<endl;
    //for(auto& ns: subindex)
    //        cout<<"  "<<ns.n<<endl;

    return subindex;

}

// Load into Struct

void extract(indset::iterator& start, pair node){

    // Dereference Start Node, Get End-Point

    yaml::ind start_v = *start;

    try {
        start_v.f(start_v.p, node); 
    } catch( exception& e ){
        throw exception("can't parse variable (%s), %s", start_v.n, e.what());
    }

    /*

    // Search for Sub-Nodes

    indset subindex = sub(start_v);

    cout<<subindex.size()<<endl;

    // Direct Extraction

    if(subindex.empty()){

        
    
    }

    if(start_v.f == &parse_array<float, 5>)
        cout<<"IS ATOMIC"<<endl;

    // Sub-Index Recursion: Struct

    else if(node.second.IsMap()){

        for(auto& sub: subindex){

            start = find(sub);

            auto it = node.second.Begin();
            while(it != node.second.End() && (*it).first != (*start).n)
                it++;

            if(it == node.second.End())
                throw exception("can't find node for key (%s)", (*start).n);

            try {
                extract(start, (*it));
            } catch( exception& e ){
                throw exception("can't parse variable (%s), %s", start_v.n, e.what());
            }

        }

    }

    // Sub-Index Recursion: Array of Struct

    else if(node.second.IsSequence()){

        cout<<"ARRAY OF STRUCT"<<endl;

        auto it = node.second.Begin();

        for(auto& sub: subindex){

            start = find(sub);

        //    cout<<(*start).n<<endl;

            auto itt = (*it).second.Begin();

            while(itt != (*it).second.End() && (*itt).first != (*start).n)
                itt++;

            try {
                extract(start, (*itt));
            } catch( exception& e ){   
                throw exception("can't parse variable (%s), %s", start_v.n, e.what());
            }

            itt++;
            if(itt == (*it).second.End()){
                it++;
            }


        }

    }

    // Non-Map Type Node

    else throw exception("invalid node for key (%s). have (%s), want (!none)", start_v.n, type(node.second.Type()));

    */

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
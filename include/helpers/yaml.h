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


I believe the only way to solve the struct problem is to correctly assign the nesting depth.

struct {
    struct {
        int;
    }
}

1. All have the same position
2. All have the same size
3. only difference is: WHAT?

independent of the order they are registered in...

and if i re-register.

*/


/*
    what happens when I register a key twice?
    once inside an initializer, once outside?

    e.g.:
    int i[5] << yaml::key("i"); // registers 6 keys WITH order
    i[0] << yaml::key("i")      // registers a key with a base order.


    I belie

    my problem is, I need to identify the depth of each component.
    then i can order the nodes correctly.

    identically named nodes at same position, size and depth
    should not be allowed? I think I can constrain more
    one of either size or depth seems redundant

    if the depth is given, I can insert with depth
    then the entrypoint always find the correct variable
    which is my current issue

    write a catch for cant parse empty struct

    OR I JUST DEFINE A STRUCT AS A TEMPLATED THING
    WITH ONE OR MULTIPLE ARGUMENTS

    AND THEN I CAN SIMPLY USE THAT TEMPLATE...
    ...nope doesn't work

    so instead:
        generate the correct depth,
        utilize the ordering to my advantage.
        then I can sort stuff by nesting depth
        the order will always be correct
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

/*
    templated object depth...
    how to do it?
*/

template <class T> // concept
concept is_class = std::is_class<T>::value;

template <class T> // concept
concept is_member = std::is_member_object_pointer<T>::value;

template<typename T>
struct mystruct {
    mystruct(T){}
    // struct of anything...
    // can be constructed by anything?
};

// I need to find a way to find its member?
// or determine whether it is a member functoin?

template<typename T>
struct depth_v : std::integral_constant<std::size_t, 0> {};

template<typename T>
struct depth_v<mystruct<T>> : std::integral_constant<std::size_t, 1 + depth_v<T>::value> {};

template<is_member T>
struct depth_v<T> : std::integral_constant<std::size_t, 1> {};

template<typename T>
inline constexpr std::size_t depth = depth_v<T>::value; // (C++17)

/*
template<typename T>
struct dimensions : std::integral_constant<std::size_t, 0> {};

template<typename T>
struct dimensions<std::vector<T>> : std::integral_constant<std::size_t, 1 + dimensions<T>::value> {};

template<typename T>
inline constexpr std::size_t dimensions_v = dimensions<T>::value; // (C++17)
*/

/*


template<typename T>
constexpr int depth(T t);

template<typename T>
constexpr int depth(T t){
    return 0;
}

template<is_class T>
constexpr int depth(T t){
    return 1;
}

*/

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

typedef std::pair<const string&, Yaml::Node&> pair;

struct ind {
    void* p;                            // Pointer to Memory
    const size_t s;                     // sizeof(value)
    const char (*t);                    // typeof(value)
    const char (*n);                    // Name of Key
    void (*f)(const yaml::ind&, pair);  // Parse Function Pointer
    const int o = 0;                    // Order / Depth
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
        if(a.o < b.o) return true;
        if(a.o > b.o) return false;
        return false;
    }
};

typedef set<ind, ind_sort> indset;

// Index Struct and Set

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
    catch(const invalid_argument& e){
        throw exception( "not float (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<double>(const yaml::ind& p, string s){
    try { new ((double*)(p.p)) double(std::stod(s)); } 
    catch(const invalid_argument& e){
        throw exception( "not double (\"%s\")", s.c_str() );
    }
}

template<>
inline void parse<string>(const yaml::ind& p, string s){
    try { new ((string*)(p.p)) string(s); } 
    catch(const invalid_argument& e){
        throw exception( "not double (\"%s\")", s.c_str() );
    }
}

// Templated Pair-Parse Functions

// Pass-Through Default Function: Interpret Node Value
//      ...and retriever!

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

//template<typename T, size_t N>
//constexpr inline void (*parser( T(&t)[N] ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( vector<T> ))(const yaml::ind&, pair);

template<typename T>
constexpr inline void (*parser( set<T> ))(const yaml::ind&, pair);

template<typename Tkey, typename Tval>
constexpr inline void (*parser( map<Tkey, Tval> ))(const yaml::ind&, pair);

// Implementations

/* Atomic Parse Means:
    Non-Default Atomic Function Call.
    We can assume that all types in here are either
    structs, arrays of structs, or scalars.
    Therefore, if the sub-index is empty, we
    can assume that something was done incorrectly.
*/

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
void insert(T& var, const yaml::ind_key& n, int order = 0){

    indset::iterator i = find(var);
    indset subindex = sameall((*i));   // Search Memory Subindex

    cout<<n.key<<endl;
    cout<<depth<T><<endl;
    cout<<subindex.size()<<endl;

    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        n.key,
        parser(var),
        order
    });

}

template<is_class T>
void insert(T& var, const yaml::ind_key& n, int order = 0){

    indset::iterator i = find(var);
    indset subindex = sameall((*i));   // Search Memory Subindex

    cout<<"class "<<n.key<<endl;
    cout<<depth<T><<endl;
    cout<<subindex.size()<<endl;

    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        n.key,
        parser(var),
        order
    });

}

// Array

template<typename T, size_t N>
void insert(T(&var)[N], const yaml::ind_key& k, int order = 0){

    yaml::index.insert(yaml::ind{
        (void*)&var,
        sizeof(var),
        typeid(var).name(),
        k.key,
        parser(var),
        order
    });

    for(size_t n = 0; n < N; n++)
        insert(var[n], key(std::to_string(n).c_str()), order + 1);

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

    indset::iterator start = yaml::find(*var);
    cout<<"START "<<(*start).n<<endl;
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
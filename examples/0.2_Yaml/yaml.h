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
*/

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "Yaml.cpp"

namespace yaml {
using namespace std;

// Constants

bool OMITEMPTY = true;

inline bool err(int N, string err){
    printf("(%d) YAML Error: %s\n", N, err.c_str());
    return false;
}

/*
===================================================
        In-Place String Atomic Type Parsing
===================================================
*/

// In-Place Atomic Parse Function

inline void parse_char(const void* p, string s){
    *((char*)p) = s.c_str()[0];
}

inline void parse_int(const void* p, string s){
    *((int*)p) = std::stoi(s);
}

inline void parse_float(const void* p, string s){
    *((float*)p) = std::stof(s);
}

inline void parse_double(const void* p, string s){
    *((double*)p) = std::stod(s);
}

inline void parse_string(const void* p, string s){
    *((string*)p) = s;
}

inline void parse_vector(const void* p, string s){
    *((string*)p) = s;
}

// Templated Function Pointer Retriever
template<typename T>
constexpr void (*parser(T))(const void*, string){
    return NULL;
}

template<>
constexpr void (*parser(char))(const void*, string){
    return &parse_char;
}

template<>
constexpr void (*parser(int))(const void*, string){
    return &parse_int;
}

template<>
constexpr void (*parser(float))(const void*, string){
    return &parse_float;
}

template<>
constexpr void (*parser(double))(const void*, string){
    return &parse_double;
}

template<>
void (*parser(string))(const void*, string){
    return &parse_string;
}

template<typename T>
void parse_vector(const void* p, string s){
    auto pp = parser(T(0));
    if(pp == NULL){
        cout<<"CANT PARSE"<<endl;
    }
    else {
        ((vector<T>*)p)->push_back(0);
        pp(&(((vector<T>*)p)->back()), s);
    }
}

template<typename T>
void (*parser(vector<T>))(const void*, string){
    return &parse_vector<T>;
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
    void (*f)(const void*, string);     // Parse Function Pointer
};

ostream& operator<<(ostream& os, const ind& i){
    os << i.p << " ";
    os << i.s << " ";
    os << i.t << " ";
    os << i.n;
    return os; 
}

// Index Storage Set

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
    return "NO KEY";
}

template<typename T>
const char* key(vector<T>* t){
    for(auto& i: index)
    if(i.p == (void*)t)
        return i.n;
    return "NO KEY";
}

template<typename T>
indset::iterator find(T* t){
    indset::iterator it = index.begin();
    for(; it != index.end(); it++)
    if((*it).p == (void*)t)
        break;
    return it;
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

bool extract(set<ind, ind_sort>& subindex, set<ind, ind_sort>::iterator& start, Yaml::Node& node){

    // Dereference Start Node, Get End-Point

    yaml::ind start_v = *start;

    // Find the YAML Node

    auto it = node.Begin();
    while(it != node.End() && (*it).first != start_v.n)
        it++;
       
    if(it == node.End()){
        cout<<"Failed to find node \""<<start_v.n<<"\""<<endl;
        return false;
    }

    // Search for Sub-Nodes

    set<ind, ind_sort> nsubindex;
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

        if(start_v.f == NULL){

            cout<<"Node has no parser"<<endl;
            return false;
        
        }

        if((*it).second.IsScalar()){

            start_v.f(start_v.p, (*it).second.As<string>()); 
            return true;

        }

        if((*it).second.IsSequence()){

            auto snode = (*it).second;

            for(auto itt = snode.Begin(); itt != snode.End(); itt++){

                if(!(*itt).second.IsScalar()){
                    cout<<"NOT SCALAR"<<endl;
                } else {
                    //cout<<(*itt).second.As<string>()<<endl;
                    start_v.f(start_v.p, (*itt).second.As<string>()); 
                }

            }

            return true;

        }

        else {
            cout<<"Node is not scalar type"<<endl;
            return false;
        }
    
    }

    // Sub-Index Recursion

    else if((*it).second.IsMap()){

        for(auto& sub: nsubindex){

            for(start = index.begin(); start != index.end(); start++)
            if((*start).p == sub.p && (*start).s == sub.s)
                break;

            if(!extract(nsubindex, start, (*it).second) && !OMITEMPTY)
                return false;

        }

    } 

    // Non-Map Type Node

    else {

        cout<<"Failed to find node \""<<start_v.n<<"\""<<endl;
        cout<<"Node is not map type"<<endl;
        return false;
    
    }

    return true;

}

template<typename T>
bool load(T* var, const char* filename){

    Yaml::Node root;

    try{
    
        Yaml::Parse(root, filename); 
    
    } catch(const Yaml::Exception e){
    
        std::cout << "Exception " << e.Type() << ": " << e.what() << std::endl;
        return false; 
    
    }

    if(root.Size() == 0){        
        cout<<"EMPTY YAML FILE"<<endl;
        return false;
    }

    set<ind, ind_sort>::iterator start;
    start = yaml::find(var);

    if(start == index.end())
        return err(0, "key not assigned");

    set<ind, ind_sort> nindex = index;
    return extract(nindex, start, root);

}

};

#endif
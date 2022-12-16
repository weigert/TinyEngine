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
#include "Yaml.cpp"

namespace yaml {
using namespace std;

// Constants

bool OMITEMPTY = false;

inline bool err(int N, string err){
    printf("(%d) YAML Error: %s\n", N, err.c_str());
    return false;
}

/*
===================================================
            In-Place String Type Parsing
===================================================
*/

// In-Place Parse Function

void parse_null(void* p, string s){
    cout<<"Can't parse \""<<s<<"\". Parser not implemented."<<endl;
}

void parse_char(void* p, string s){
    *((char*)p) = s.c_str()[0];
}

void parse_int(void* p, string s){
    *((int*)p) = std::stoi(s);
}

void parse_float(void* p, string s){
    *((float*)p) = std::stof(s);
}

void parse_double(void* p, string s){
    *((double*)p) = std::stod(s);
}

// Templated Function Pointer Retriever

template<typename T>
void (*parser(T))(void*, string){
    return &parse_null;
}

template<>
void (*parser(char))(void*, string){
    return &parse_char;
}

template<>
void (*parser(int))(void*, string){
    return &parse_int;
}

template<>
void (*parser(float))(void*, string){
    return &parse_float;
}

template<>
void (*parser(double))(void*, string){
    return &parse_double;
}

/*
===================================================
        Raw Memory Index / Key Assignment
===================================================
*/

// Index Struct and Set

struct ind {
    void* p;                    // Pointer to Memory
    size_t s;                   // sizeof(value)
    const char (*t);            // typeof(value)
    const char (*n);            // Name of Key
    void (*f)(void*, string);   // Parse Function Pointer
};

ostream& operator<<(ostream& os, const ind& i){
    os << i.p << " ";
    os << i.s << " ";
    os << i.t << " ";
    os << i.n;
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

set<ind, ind_sort> index;

// Index Construction Helper

struct ind_key {
    const char* key;
};

ind_key key(const char* key){
    return ind_key{key};
}

template<typename T>
const char* key(T* t){
    for(auto& i: index){
        if(i.p == (void*)t)
            return i.n;
    }
    return "NO KEY";
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

    if(nsubindex.empty())
        start_v.f(start_v.p, (*it).second.As<string>()); 

    else for(auto& sub: nsubindex){

        for(start = index.begin(); start != index.end(); start++)
        if((*start).p == sub.p && (*start).s == sub.s)
            break;

        extract(nsubindex, start, (*it).second);

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
    for(start = index.begin(); start != index.end(); start++)
    if((*start).p == (void*)var)
        break;

    if(start == index.end())
        return err(0, "key not assigned");

    set<ind, ind_sort> nindex = index;
    return extract(nindex, start, root);

}

};

#endif
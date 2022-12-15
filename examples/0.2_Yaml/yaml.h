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

void parse_int(void* p, string s){
    *((int*)p) = std::stoi(s);
}

void parse_double(void* p, string s){
    *((double*)p) = std::stod(s);
}

// Templated Function Pointer Retriever

template<typename T>
void (*parser(T))(void*, string){
    return NULL;
}

template<>
void (*parser(int))(void*, string){
    return &parse_int;
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

template<typename T>
bool load(T* var, const char* filename){

    // Find the Root Element

    yaml::ind* root = NULL;

    for(yaml::ind i: yaml::index){
        if(i.p == (void*)var){
            root = &i;
            break;
        }
    }

    if(root == NULL)
        return err(0, "key not assigned");

    // Parse the File
    size_t N = 0;
    bool found = false;

    parse::cstream ls(filename);
    cout<<ls<<endl;
    parse::tstream ts(ls);
    cout<<ts<<endl;

  //  parse::node::node* n = parse::node::parse(ts);
  //  delete n;

    /*


    ifstream f(filename);
    string line;
    size_t pos;

    while(getline(f, line)){

        N++;

        // Remove Comments
        pos = line.find("#");
        if(pos != string::npos)
            line = line.substr(0, pos);

        // Skip Empty Lines
        if(line.size() == 0)
            continue;

        // Find the Token
        pos = line.find(":");
        if(pos == string::npos)
            return err(N, "missing \":\" token");

        // Find the Key, Value Pair
        string key = line.substr(0, pos);
        if(key.size() == 0)
            return err(N, "missing key");

        string val = line.substr(pos+1, line.size());
        if(key.size() == 0)
            return err(N, "missing value");

        if(key == string(root->n)){

            if(found == true){
                return err(N, "duplicate token \""+key+"\"");
            }

            found = true;
            root->f(root->p, val);
        }

    } 

    // Check for OMITEMPTY

    if(!OMITEMPTY && !found)
        return err(N, "value not found for key \"" + string(root->n) + "\"");

    */

    /*
        I can take the address of var,
        and get the set of elements in its memory.

        Then I utilize the pointer nesting to determine
        which element gets loaded into where.

        The problem is, I need a function to convert
        a string to a specfic type as a callback,
        so I can place the correct data at the pointer location.
        The question is, can I utilize a function pointer?
    */

    // Find the Element in the set which is in range...
    // or rather, the element at the set which has the EXACT pointer value.
    // note that there could be multiple.

    // Like what if I want to fill the element root.a?
    // Then it should assume that the element a

    // When do I have pointer collisions?
    // ONLY if the element is a struct!!!!
    // So I will have maximally nested structs.

    // Trying to load STRUCTA.STRUCTB.VALUE
    // I would have to somehow decide to use

    // Note: I can not have the same struct type nested inside.
    // So I can simply check the type for the correct value.



    return true;

}

};

#endif
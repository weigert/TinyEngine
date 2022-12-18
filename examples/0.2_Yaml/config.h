/*
    TinyEngine Direct-To-Struct
    YAML Loading Test-Suite
*/

// Basic Types

int i = 0;
float f = 0.0;
double d = 0.0; 
char c = 'a';
std::string s = "";

// Structs / Nested Structs

struct StructA {

    int a; 
    double b;
    float c;
    double d;

    StructA(){
        d << yaml::key("d");
        b << yaml::key("b");
        a << yaml::key("a");
    }

} A;

struct StructB {

    char c;
    StructA A;

    StructB(){
        c << yaml::key("c");
        A << yaml::key("A");
    }

} B;

struct StructC {

    StructB b;
    char g;

    StructC(){
        g << yaml::key("g");
        b << yaml::key("b");
    }

} C;

// C++ Arrays

float farr[5]{0.5};
int iarr[2]{0};

struct StructArr {
    char a;
    StructArr(){
        a << yaml::key("a");
    }

};

StructArr structarr[3];

// STL Container Types

std::vector<int> v;
std::vector<std::vector<int>> vv;

std::set<int> iset;
std::set<std::string> sset;

std::map<int, int> iimap;
std::map<std::string, int> simap;
std::map<int, std::string> ismap;

std::map<int, std::vector<int>> miv;

struct VecStruct {
    std::vector<int> v;
    int a;
    VecStruct(){
        v << yaml::key("v");
    }
} vecstruct;


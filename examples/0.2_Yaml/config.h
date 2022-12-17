/*
    TinyEngine Direct-To-Struct
    YAML Loading Test-Suite
*/

// Basic Types

int i = 0;
float f = 0.0;
double d = 0.0; 
char c;
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

    StructA A;
    char c;

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

// STL Container Types

std::vector<int> v;

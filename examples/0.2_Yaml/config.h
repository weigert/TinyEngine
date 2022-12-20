/*
    TinyEngine Direct-To-Struct
    YAML Loading Test-Suite
*/

/*
====================================================
                Basic Type Parsing
====================================================
*/

void parse_basic(){

    int i = 0;
    i << yaml::key("integer");
    if(!yaml::load(&i, "config/basic.yaml"))
        std::cout<<"Failed to load config/basic.yaml"<<std::endl;
    std::cout<<yaml::find(i)<<" = "<<i<<std::endl;

    float f = 0.0;
    f << yaml::key("float");
    if(!yaml::load(&f, "config/basic.yaml"))
        std::cout<<"Failed to load config/basic.yaml"<<std::endl;
    std::cout<<yaml::find(f)<<" = "<<f<<std::endl;

    double d = 0.0; 
    d << yaml::key("double");
    if(!yaml::load(&d, "config/basic.yaml"))
        std::cout<<"Failed to load config/basic.yaml"<<std::endl;
    std::cout<<yaml::find(d)<<" = "<<d<<std::endl;

    char c = 'a';
    c << yaml::key("char");
    if(!yaml::load(&c, "config/basic.yaml"))
        std::cout<<"Failed to load config/basic.yaml"<<std::endl;
    std::cout<<yaml::find(c)<<" = "<<c<<std::endl;

    std::string s = "";
    s << yaml::key("string");
    if(!yaml::load(&s, "config/basic.yaml"))
        std::cout<<"Failed to load config/basic.yaml"<<std::endl;
    std::cout<<yaml::find(s)<<" = "<<s<<std::endl;

}

/*
====================================================
                GLM Type Parsing
====================================================
*/

void parse_glm(){

    glm::ivec2 vec2;
    vec2 << yaml::key("vec2");
    if(!yaml::load(&vec2, "config/glm.yaml"))
        std::cout<<"Failed to load config/glm.yaml"<<std::endl;

    glm::ivec3 vec3;
    vec3 << yaml::key("vec3");
    if(!yaml::load(&vec3, "config/glm.yaml"))
        std::cout<<"Failed to load config/glm.yaml"<<std::endl;

    glm::ivec4 vec4;
    vec4 << yaml::key("vec4");
    if(!yaml::load(&vec4, "config/glm.yaml"))
        std::cout<<"Failed to load config/glm.yaml"<<std::endl;

    glm::mat2 mat2;
    mat2 << yaml::key("mat2");
    if(!yaml::load(&mat2, "config/glm.yaml"))
        std::cout<<"Failed to load config/glm.yaml"<<std::endl;
    std::cout<<"mat2"<<std::endl;
    std::cout<<"  "<<mat2[0][0]<<std::endl;
    std::cout<<"  "<<mat2[0][1]<<std::endl;
    std::cout<<"  "<<mat2[1][0]<<std::endl;
    std::cout<<"  "<<mat2[1][1]<<std::endl;

    glm::mat3 mat3;
    mat3 << yaml::key("mat3");
    if(!yaml::load(&mat3, "config/glm.yaml"))
        std::cout<<"Failed to load config/glm.yaml"<<std::endl;
    std::cout<<"mat3"<<std::endl;
    std::cout<<"  "<<mat3[0][0]<<std::endl;
    std::cout<<"  "<<mat3[0][1]<<std::endl;
    std::cout<<"  "<<mat3[0][2]<<std::endl;
    std::cout<<"  "<<mat3[1][0]<<std::endl;
    std::cout<<"  "<<mat3[1][1]<<std::endl;
    std::cout<<"  "<<mat3[1][2]<<std::endl;
    std::cout<<"  "<<mat3[2][0]<<std::endl;
    std::cout<<"  "<<mat3[2][1]<<std::endl;
    std::cout<<"  "<<mat3[2][2]<<std::endl;

    glm::mat4 mat4;
    mat4 << yaml::key("mat4");
    if(!yaml::load(&mat4, "config/glm.yaml"))
        std::cout<<"Failed to load config/glm.yaml"<<std::endl;
    std::cout<<"mat4"<<std::endl;
    std::cout<<"  "<<mat4[0][0]<<std::endl;
    std::cout<<"  "<<mat4[0][1]<<std::endl;
    std::cout<<"  "<<mat4[1][0]<<std::endl;
    std::cout<<"  "<<mat4[1][1]<<std::endl;

}

/*
====================================================
                STL Container Parsing
====================================================
*/

void parse_stl(){

    // Vector

    std::vector<int> v;
    v << yaml::key("vector");
    if(!yaml::load(&v, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;
    std::cout<<"vector of int:"<<std::endl;
    for(auto& vec: v)
        std::cout<<"  "<<vec<<std::endl;

    // Set

    std::set<std::string> sset;
    sset << yaml::key("sset");
    if(!yaml::load(&sset, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"set of string:"<<std::endl;
    for(auto& s: sset)
        std::cout<<"  "<<s<<std::endl;

    std::set<int> iset;
    iset << yaml::key("iset");
    if(!yaml::load(&iset, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"set of int:"<<std::endl;
    for(auto& i: iset)
        std::cout<<"  "<<i<<std::endl;

    // Map

    std::map<int, int> iimap;
    iimap << yaml::key("iimap");
    if(!yaml::load(&iimap, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"integer map of integer:"<<std::endl;
    for(auto& ii: iimap){
        std::cout<<"  "<<ii.first<<" "<<ii.second<<std::endl;
    }

    std::map<int, std::string> ismap;
    ismap << yaml::key("ismap");
    if(!yaml::load(&ismap, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"integer map of string:"<<std::endl;
    for(auto& ii: ismap){
        std::cout<<"  "<<ii.first<<" "<<ii.second<<std::endl;
    }

    std::map<std::string, int> simap;
    simap << yaml::key("simap");
    if(!yaml::load(&simap, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"string map of integer:"<<std::endl;
    for(auto& ii: simap){
        std::cout<<"  "<<ii.first<<" "<<ii.second<<std::endl;
    }
    
    // Vector of Vector

    std::vector<std::vector<int>> vv;
    vv << yaml::key("vv");
    if(!yaml::load(&vv, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"vector of vector:"<<std::endl;
    for(auto& v: vv){
        for(auto& vec: v){
            std::cout<<"  "<<vec<<std::endl;
        }
        std::cout<<std::endl;
    }

    // Map of Vector

    std::map<int, std::vector<int>> miv;
    miv << yaml::key("miv");
    if(!yaml::load(&miv, "config/stl.yaml"))
        std::cout<<"Failed to load config/stl.yaml"<<std::endl;

    std::cout<<"integer map of vector:"<<std::endl;
    for(auto& m: miv){
        std::cout<<m.first<<std::endl;
        for(auto& vec: m.second){
            std::cout<<"  "<<vec<<std::endl;
        }
    }

}

/*
====================================================
                Struct / Array Parsing
====================================================
*/

void parse_array(){

    // C++ Arrays

    float farr[5];
    farr << yaml::key("float_array");
    if(!yaml::load(&farr, "config/array.yaml"))
        std::cout<<"Failed to load config/array.yaml"<<std::endl;

    std::cout<<"array of float [5]:"<<std::endl;
    for(size_t i = 0; i < 5; i++)
        std::cout<<"  "<<farr[i]<<std::endl;

    int iarr[2];
    iarr << yaml::key("integer_array");
    if(!yaml::load(&iarr, "config/array.yaml"))
        std::cout<<"Failed to load config/array.yaml"<<std::endl;

    std::cout<<"array of integer[2]:"<<std::endl;
    for(size_t i = 0; i < 2; i++)
        std::cout<<"  "<<iarr[i]<<std::endl;

    // Multi-Dimensional Arrays

    float multi_arr[2][3];
    multi_arr << yaml::key("multi_array");
    if(!yaml::load(&multi_arr, "config/array.yaml"))
        std::cout<<"Failed to load config/array.yaml"<<std::endl;
    std::cout<<"array of float[2][3]:"<<std::endl;
    for(size_t i = 0; i < 2; i++)
    for(size_t j = 0; j < 3; j++)
        std::cout<<"  "<<multi_arr[i][j]<<std::endl;

    int multi_arr_single[2][1][3];
    multi_arr_single << yaml::key("multi_array_single");
    if(!yaml::load(&multi_arr_single, "config/array.yaml"))
        std::cout<<"Failed to load config/array.yaml"<<std::endl;
    std::cout<<"array of int[2][1][3]:"<<std::endl;
    for(size_t i = 0; i < 2; i++)
    for(size_t j = 0; j < 1; j++)
    for(size_t k = 0; k < 3; k++)
        std::cout<<"  "<<multi_arr_single[i][j][k]<<std::endl;

}

void parse_struct(){

    // C++ Structs

    struct StructS {
        int s;
        StructS(){
        }
    } S;
    
                S.s << yaml::key("s");
    S << yaml::key("StructS");

    if(!yaml::load(&S, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;
    std::cout<<"S"<<std::endl;
    std::cout<<"    s = "<<S.s<<std::endl;

    struct StructSS {
        StructS S;
        StructSS(){
        }
    } SS;
    
    SS.S.s << yaml::key("s");
    SS.S << yaml::key("StructS");
    SS << yaml::key("StructSS");

    if(!yaml::load(&SS, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;
    std::cout<<"SS"<<std::endl;
    std::cout<<"    SS.S"<<std::endl;
    std::cout<<"        SS.S.s = "<<SS.S.s<<std::endl;

    // Array of Struct


    struct StructA {
        int a;
        double b;
        StructA(){
            a << yaml::key("a");
            b << yaml::key("b");
        }
    } A;

    StructA StructA_arr[1];
    StructA_arr << yaml::key("StructA_arr");
    if(!yaml::load(&StructA_arr, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;
    for(int i = 0; i < 1; i++){
        std::cout<<"A["<<i<<"]"<<std::endl;
            std::cout<<"    a = "<<StructA_arr[i].a<<std::endl;
            std::cout<<"    b = "<<StructA_arr[i].b<<std::endl;
    }

    return;

    A << yaml::key("StructA");
    if(!yaml::load(&A, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;
    std::cout<<"A"<<std::endl;
    std::cout<<"    a = "<<A.a<<std::endl;
    std::cout<<"    b = "<<A.b<<std::endl;

    struct StructB {

        StructA A;
        char c;

        StructB(){
            A << yaml::key("A");
            c << yaml::key("c");
        }

    } B;

    B << yaml::key("StructB");
    if(!yaml::load(&B, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;
    std::cout<<"B"<<std::endl;
    std::cout<<"    A"<<std::endl;
    std::cout<<"        a = "<<B.A.a<<std::endl;
    std::cout<<"        b = "<<B.A.b<<std::endl;
    std::cout<<"    c = "<<B.c<<std::endl;

    struct StructC {

        StructB B;
        char d;

        StructC(){
            B << yaml::key("B");
            d << yaml::key("d");
        }

    } C;

    C << yaml::key("StructC");
    if(!yaml::load(&C, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;
    std::cout<<"C"<<std::endl;
    std::cout<<"    B"<<std::endl;
    std::cout<<"        A"<<std::endl;
    std::cout<<"            a = "<<C.B.A.a<<std::endl;
    std::cout<<"            b = "<<C.B.A.b<<std::endl;
    std::cout<<"        c = "<<C.B.c<<std::endl;
    std::cout<<"    d = "<<C.d<<std::endl;



}

    /*

// Structs / Nested Structs



// STL Container Types




    // Structs / Nested Structs


    // Array of Struct

    structarr << yaml::key("struct_array");
    if(!yaml::load(&structarr, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;

    std::cout<<"START"<<std::endl;

    std::cout<<"structarr:"<<std::endl;
    for(size_t i = 0; i < 3; i++){
        std::cout<<"  "<<structarr[i].a<<std::endl;
        std::cout<<"  "<<structarr[i].b<<std::endl;
        std::cout<<"  "<<structarr[i].c<<std::endl;
    }

    */

void parse_struct_array(){

    // Struct of Array

    struct ArrStruct {
        int a[4];
        ArrStruct(){
            a << yaml::key("a");
        }
    } arrstruct;

    arrstruct << yaml::key("array_struct");
    if(!yaml::load(&arrstruct, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;

    std::cout<<"array of struct:"<<std::endl;
    for(int i = 0; i < 4; i++)
        std::cout<<"  "<<arrstruct.a[i]<<std::endl;

    // Struct of STL Container

    struct VecStruct {
        std::vector<int> v;
        int a;
        VecStruct(){
            v << yaml::key("v");
        }
    } vecstruct;

    vecstruct << yaml::key("vector_struct");
    if(!yaml::load(&vecstruct, "config/struct.yaml"))
        std::cout<<"Failed to load config/struct.yaml"<<std::endl;

    std::cout<<"struct of vector:"<<std::endl;
    for(auto& ii: vecstruct.v){
        std::cout<<"  "<<ii<<std::endl;
    }

}

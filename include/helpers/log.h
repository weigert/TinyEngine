#include <cstdarg>
#include <string>

namespace logg{
using namespace std;

// Logger Metadata

bool debug = true;
string key = "LOG";

// Raw Data Output

void raw(){ cout<<endl; }

template <typename T>
void raw(T m){ cout<<m<<endl; }

template <typename T, typename... Types>
void raw(T m, Types... r){ cout<<m; raw(r...); }

// Wrapped Data Output

template <typename T>
void out(T m){ cout<<"["<<key<<"] "; raw(m); }

template <typename T, typename... Types>
void out(T m, Types... r){ cout<<"["<<key<<"] "; raw(m, r...); }

// Error Data Otput

template <typename T>
void err(T m){ cout<<"[ERR] "; raw(m); }

template <typename T, typename... Types>
void err(T m, Types... r){ cout<<"[ERR] "; raw(m, r...); }


template <typename... Types>
void deb(Types... r){
  if(!debug) return;
  cout<<"[Debug] "; raw(r...);
}

template <typename... Types>
void deb(bool f, Types... r){
  if(!f) return;  //Failed Debug Condition
  cout<<"[Debug] "; raw(r...);
}

template <typename T>
void progress(T d, T D){
  if(d != 0) std::cout<<"\r";
  std::cout<<"["<<d<<"/"<<D<<"]";

  for(int i = 0; i < 25; i++){
    if(i < 25*((float)d/(float)D)) std::cout<<"#";
    else std::cout<<"-";
  }
  std::cout<<"]";
  if(d == D-1) printf("\n");
  std::cout<<std::flush;
}

};

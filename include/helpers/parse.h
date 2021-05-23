#ifndef TINYENGINE_PARSE
#define TINYENGINE_PARSE

#include <map>
#include <unordered_set>
#include <string>
#include <vector>

namespace parse{
using namespace std;

string program = "";
vector<string> val;
unordered_set<string> flag;
map<string, string> option;

void get(int n, char* args[]){
  string arg;
  for(int i = 0; i < n; i++){
    arg = string(args[i]);

    if(i == 0){
      program = arg;
      continue;
    }

    if(arg.substr(0,2) == "--"){
      flag.insert(arg.substr(2));
    }
    else if(arg.substr(0,1) == "-"){
      if(i == n-1) return;
      option[arg.substr(1)] = string(args[++i]);
    }
    else{
      val.push_back(arg);
    }

  }
}

}

#endif

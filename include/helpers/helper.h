template<typename T>
T min(T a, T b){
    return (a < b)?a:b;
}

template<typename T>
T max(T a, T b){
    return (a > b)?a:b;
}

template<typename T>
T clamp(T t, T a, T b){
    return (a > b)?a:b;
}

std::hash<std::string> position_hash;
float hashrand(int i){
  return (float)(position_hash(std::to_string(i))%1000)/1000.0f;
}

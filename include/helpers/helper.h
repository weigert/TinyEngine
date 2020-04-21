/*
  Generally Useful Functions
*/

template<typename T>
T min(T a, T b){
    return (a < b)?a:b;
};

template<typename T>
T max(T a, T b){
    return (a > b)?a:b;
};

template<typename T>
T clamp(T t, T a, T b){
    return (a > b)?a:b;
};

struct Buffer{
  unsigned int n = 0;
  const int max = 256;
  char raw[256] = {0};
};

std::ostream& operator<<(std::ostream& os, const Buffer& b){
  for(int i = 0; i < b.max; i++)
    os << b.raw[i];
  return os;
}

namespace buffer {

//ENCODE

template <typename T>
size_t encode(char* raw, const T m){
  memcpy(raw, &m, sizeof(T));
  return sizeof(T);
}

template<>
size_t encode(char* raw, const char* m){
  memcpy(raw, m, strlen(m)+1);
  return strlen(m) + 1;
}

template<>
size_t encode(char* raw, const std::string m){
  return encode(raw, m.c_str());
}

template <typename T, typename... Types>
void encode(char* raw, const T m, const Types... r){
  encode(raw + encode(raw, m), r...);
}

//DECODE (for some reason array decode just works)

template <typename T>
size_t decode(char* raw, T& m){
  memcpy(&m, raw, sizeof(T));
  return sizeof(T);
};

template <>
size_t decode(char* raw, std::string& m){
  char a = '0';
  m.clear();
  for(int i = 0; i < 256 && a != '\0'; ++i){
    memcpy(&a, raw + i*sizeof(char), 1);
    m.push_back(a);
  }
  return strlen(m.c_str())+1;
};

template <typename T, typename... Types>
void decode(char* raw, T& m, Types&... r){
  decode(raw + decode(raw, m), r...);
};

//KEY

std::string key(Buffer* buf){
  std::string out;
  size_t shift = decode(buf->raw, out);               //Extract String
  memmove(buf->raw, buf->raw+shift, buf->max-shift);  //Slice
  return out;
};


};

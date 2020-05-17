namespace buffer {
  const int max = 256;
};

struct Buffer{
  size_t n = 0;
  char raw[buffer::max] = {0};
  void flush(){ n = 0; }
};

namespace buffer {
  template <typename T>
  void encode(Buffer* b, const T m){
    size_t l = sizeof(T);
    if(b->n + l > max) //Buffer Overflow
      l  = max - b->n;
    memcpy(b->raw + b->n, &m, l);
    b->n += l;
  }

  template<>
  void encode(Buffer* b, const char* m){
    size_t l = strlen(m)+1;
    if(b->n + l > max) //Buffer Overflow
      l  = max - b->n;
    memcpy(b->raw + b->n, m, l);
    b->n += l;
  }

  template<>
  void encode(Buffer* b, const std::string m){
    encode(b, m.c_str());
  }

  template <typename T, typename... Types>
  void encode(Buffer* b, const T m, const Types... r){
    encode(b, m);
    encode(b, r...);
  }

  template <typename T>
  void decode(Buffer* b, T& m){
    memcpy(&m, b->raw + b->n, sizeof(T));
    b->n += sizeof(T);
  };

  template <>
  void decode(Buffer* b, std::string& m){
    m.clear();
    for(char a = '0'; b->n < max && a != '\0'; b->n++){
      memcpy(&a, b->raw + b->n, 1);
      m.push_back(a);
    }
  };

  template <typename T, typename... Types>
  void decode(Buffer* b, T& m, Types&... r){
    decode(b, m);
    decode(b, r...);
  };

  std::string key(Buffer* buf, bool slice = true){  //Extract Key String
    std::string out;
    decode(buf, out);                                         //Extract Key
    if(slice) memmove(buf->raw, buf->raw+buf->n, max-buf->n); //Slice
    buf->flush();                                             //Flush (Important)
    return out;
  };
};

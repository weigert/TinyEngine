namespace buffer {
  const int max = 256;
  char* start;
};

struct Buffer{
  size_t n = 0;
  char raw[buffer::max] = {0};

  void flush(){
    n = 0; //i.e. we can start overwriting from the start
  }
};

std::ostream& operator<<(std::ostream& os, const Buffer& b){
  for(int i = 0; i < buffer::max; i++)
    os << b.raw[i];
  return os;
}

namespace buffer {

  template <typename T>
  void encode(Buffer* b, const T m){
    size_t l;
    if(b->n + sizeof(T) > max){
      logger::debug("Buffer Overflow");
      l  = max - b->n;
    }
    else l = sizeof(T);

    memcpy(b->raw + b->n, &m, l);
    b->n += l;
  }

  template<>
  void encode(Buffer* b, const char* m){
    size_t l;
    if(b->n + strlen(m)+1 > max){
      logger::debug("Buffer Overflow");
      l  = max - b->n;
    }
    else l = strlen(m)+1;

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
    char a = '0';
    m.clear();
    for(; b->n < max && a != '\0'; b->n++){
      memcpy(&a, b->raw + b->n, 1);
      m.push_back(a);
    }
    m.push_back('\0');
  };

  template <typename T, typename... Types>
  void decode(Buffer* b, T& m, Types&... r){
    decode(b, m);
    decode(b, r...);
  };

  //KEY

  std::string key(Buffer* buf){
    std::string out;
    decode(buf, out);                                     //Extract Key
    memmove(buf->raw, buf->raw + buf->n, max - buf->n);   //Slice
    buf->flush();                                         //Flush (Important)
    return out;
  };

  //Sanity Check (i.e. are buffers broken?)

  void check(){
    //Sanity Check!
    Buffer* A = new Buffer();
    buffer::encode(A, "TEST", 1, 2, 3.14159f, "WORKS");
    int a, b = 0;
    float c = 0.0f;
    std::string x, y = "Nothing...";

    A->flush(); //Needs to be reset!!!!! for the sanity check
    buffer::decode(A, x, a, b, c, y);
    logger::write(x, a, b, c, y);
    delete A;
  }

};

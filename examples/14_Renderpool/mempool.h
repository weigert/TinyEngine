
//Object to Store
struct someobject {
  int val;
  someobject(int val_in):val(val_in){}
};

using namespace std;

template<typename T>
class MemPool{

public:
T* start;
size_t size;
stack<T*> free;

  MemPool(const size_t N):size(N){            //Fixed Size Allocation
    start = (T*)(::operator new(size*sizeof(T)));
    for(size_t i = 0; i < size; i++)
      free.push(start+i);
  }

  MemPool(const MemPool &)=delete;  //Copy Constructor
  MemPool(MemPool && other){        //Move Constructor
    this->free(move(other.free));
  }

  ~MemPool(){
    if(free.size() != size) std::cout<<"Not all memory has been deallocated"<<std::endl;
    ::operator delete((void*)start, size*sizeof(T));
    while(!free.empty())
      free.pop();
  }

  template<typename... Args>
  T* create(Args && ...args){
    T* place = (T*)(allocate());
    try{ new (place) T(std::forward<Args>(args)...); }
    catch(...) { free.push(place); throw; }
    return place;
  }

  void remove(T* o){
    o->~T();
    free.push(o);
  }

  //Raw Allocation
  void* allocate(){
    void* place;
    if(!free.empty()){
      place = static_cast<void*>(free.top());
      free.pop();
    }
    else{
      std::cout<<"Need to allocate new memory"<<std::endl;
      //Need to allocate new memory
      place = ::operator new(sizeof(T));
    }
    return place;
  }

  void deallocate(void* o){
    free.push(static_cast<T*>(o));
  }

};

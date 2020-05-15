#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "buffer.cpp"
#include "socket.cpp"

class Net {
public:
  Socket sock;

  std::deque<Buffer*> messages;

  ~Net(){
    close();
  }

  volatile bool listening = false;
  std::thread listener;

  bool host(int port){
    if(!sock.host(port))
      return false;

    listen();
    return true;
  }

  bool connect(std::string address, int port){
    if(!sock.connect(address, port))
      return false;

    listen();
    return true;
  }

  void input(){

    /*
        Important: buffer objects should be taken from a pool
        the received message also needs to say who sent it.
        it can then be cross-validated.
    */

    Buffer* b = new Buffer();
    while(listening){
      if(sock.receive(b, sock.target) > 0){
        messages.push_front(b);
        b = new Buffer();
      }
    }

  };

  void listen(){
    listening = true;
    listener = std::thread(&Net::input, this);
  };

  void close(){
    listening = false;
    if (listener.joinable())
      listener.join();
  };

  std::unordered_map<std::string, std::function<void(Buffer* b)>> handles;

  void handle(){

    while(!messages.empty()){

      std::string key(buffer::key(messages.back()).c_str());

      if(handles.find(key) != handles.end())  //Check for Key
        (handles[key])(messages.back());      //Execute

      delete messages.back(); //Pooling will fix this
      messages.pop_back();

    }

  };

};

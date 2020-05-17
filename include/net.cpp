struct Message{

  Message(Buffer* _b, sockaddr_in _f){
    b = _b;
    key = buffer::key(b).c_str();
    from = _f;
  }

  std::string key;
  Buffer* b;
  sockaddr_in from;

};

class Net {
public:

  Socket sock;
  int port = 7777;
  std::deque<Message> messages;

  sockaddr_in server;
  std::vector<sockaddr_in> clients;

  ~Net(){
    close();
  }

  bool host();
  bool connect(std::string address);

  volatile bool open = false;
  std::thread listener;
  void input();   //Receive a message
  void listen();  //Spawn listener thread
  void close();   //Stop listening

  std::unordered_map<std::string, std::function<void(Message m)>> handler;
  void handle();
};

bool Net::host(){
  if(!sock.open(INADDR_ANY, port))
    return false;
  if(!sock.sockbind())
    return false;

  listen();
  return true;
}

bool Net::connect(std::string address){
  if(!sock.open(inet_addr(address.c_str()), port))
    return false;

  server = sock.connection;
  listen();
  return true;
}

void Net::input(){
  Buffer* b = new Buffer(); //Actually get from a memory pool
  while(open){
    if(sock.receive(b, sock.connection) > 0){
      messages.push_front(Message(b, sock.connection));
      b = new Buffer();
    }
  }
};

void Net::listen(){
  open = true;
  listener = std::thread(&Net::input, this);
};

void Net::close(){
  open = false;
  if (listener.joinable())
    listener.join();
};

void Net::handle(){
  while(!messages.empty()){
    if(handler.find(messages.back().key) != handler.end())
      (handler[messages.back().key])(messages.back());
    delete messages.back().b; //Actually: Return to Pool
    messages.pop_back();
  }
};

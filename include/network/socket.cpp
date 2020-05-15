
class Socket {
public:

  int sockfd;
  sockaddr_in local, target;
  unsigned int len;

  ~Socket(){
    close(sockfd);
  }

  bool host(int port){

    if(!(sockfd = socket(AF_INET, SOCK_DGRAM, 0))){
      return false;
    }

    memset(&local, 0, sizeof(local));
    memset(&target, 0, sizeof(target));

    local.sin_family = AF_INET; //IPv4
    local.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY
    local.sin_port = htons(port);

    if ( bind(sockfd, (const struct sockaddr *)&local, sizeof(local)) < 0 ) {
      std::cout<<"Bind Failed"<<std::endl;
      return false;
    }

    std::cout<<"Hosting on port "<<port<<std::endl;
    return true;
  }

  bool connect(std::string address, int port){

    if(!(sockfd = socket(AF_INET, SOCK_DGRAM, 0))){
      return false;
    }

    memset(&local, 0, sizeof(local));
    memset(&target, 0, sizeof(target));

    target.sin_family       = AF_INET;                      // IPv4
    target.sin_addr.s_addr  = inet_addr(address.c_str());   //INADDR_ANY;
    target.sin_port         = htons(port);

    std::cout<<"Connecting to "<<address<<":"<<port<<std::endl;
    return true;
  }

  void send(Buffer* buf, sockaddr_in addr){
    sendto(sockfd, (char *)buf->raw, buf->max, MSG_CONFIRM, (const struct sockaddr *) &addr, sizeof(addr));
  }

  int receive(Buffer* buf, sockaddr_in& addr){
    len = sizeof(addr);
    return recvfrom(sockfd, (char *)buf->raw, buf->max, MSG_DONTWAIT, (struct sockaddr *) &addr, &len);
  }
};

class Socket {
public:

  int sockfd;
  sockaddr_in connection;
  unsigned int len;

  ~Socket(){
    close(sockfd);
  }

  bool host(int port){

    if(!(sockfd = socket(AF_INET, SOCK_DGRAM, 0))){
      return false;
    }

    memset(&connection, 0, sizeof(connection));
    connection.sin_family = AF_INET;
    connection.sin_addr.s_addr = INADDR_ANY;
    connection.sin_port = htons(port);

    if ( bind(sockfd, (const struct sockaddr *)&connection, sizeof(connection)) < 0 ) {
      std::cout<<"Bind Failed"<<std::endl;
      return false;
    }

    std::cout<<"Listening on "<<port<<std::endl;
    return true;
  }

  bool connect(std::string address, int port){

    if(!(sockfd = socket(AF_INET, SOCK_DGRAM, 0))){
      return false;
    }

    memset(&connection, 0, sizeof(connection));

    connection.sin_family       = AF_INET;                      // IPv4
    connection.sin_addr.s_addr  = inet_addr(address.c_str());   //INADDR_ANY;
    connection.sin_port         = htons(port);

    std::cout<<"Communicating with "<<address<<":"<<port<<std::endl;
    return true;
  }

  void send(Buffer* buf, sockaddr_in addr){
    sendto(sockfd, (char *)buf->raw, buffer::max, MSG_CONFIRM, (const struct sockaddr *) &addr, sizeof(addr));
  }

  int receive(Buffer* buf, sockaddr_in& addr){
    len = sizeof(addr);
    return recvfrom(sockfd, (char *)buf->raw, buffer::max, MSG_DONTWAIT, (struct sockaddr *) &addr, &len);
  }
};

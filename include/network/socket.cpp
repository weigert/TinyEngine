class Socket {
public:

  unsigned int fd, len;
  sockaddr_in connection;

  ~Socket(){
    close(fd);
  }

  bool open(in_addr_t addr, unsigned int port){
    if(!(fd = socket(AF_INET, SOCK_DGRAM, 0)))
      return false;

    memset(&connection, 0, sizeof(connection));
    connection.sin_family = AF_INET;
    connection.sin_addr.s_addr = addr;
    connection.sin_port = htons(port);

    return true;
  }

  bool sockbind(){
    return ( bind(fd, (const sockaddr *)&connection, sizeof(connection)) < 0 );
  }

  void send(Buffer* buf, sockaddr_in addr){
    sendto(fd, (char *)buf->raw, buffer::max, MSG_CONFIRM, (const struct sockaddr *) &addr, sizeof(addr));
  }

  int receive(Buffer* buf, sockaddr_in& addr){
    len = sizeof(addr);
    return recvfrom(fd, (char *)buf->raw, buffer::max, MSG_DONTWAIT, (struct sockaddr *) &addr, &len);
  }
};

#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	int port = 7777;
	if(!parse::input(argc, args, port)){
		logger::error("Couldn't parse arguments");
		return 0;
	}

	Net server;

	//Define Handlers
	server.handles["PING"] = [&](Buffer* b){
		std::cout<<"PING"<<std::endl;
		buffer::encode(b->raw, "PONG");
		server.sock.send(b, server.sock.target);
	};

	server.handles["MSG"] = [&](Buffer* b){
		std::cout<<"New Message: ";
		std::string msg;
		buffer::decode(b->raw, msg);
		std::cout<<msg<<std::endl;
	};

	server.handles["SHUTDOWN"] = [&](Buffer* b){
		server.close();
	};

	//Launch the Host
	server.host(port);

	while(server.listening){
		server.handle();
	}

	return 0;
}

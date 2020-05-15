#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	int port = 7777;
	if(!parse::input(argc, args, port)){
		logger::error("Couldn't parse arguments");
		return 0;
	}

	Net client;

	client.handles["PONG"] = [&](Buffer* b){
		std::cout<<"PONG"<<std::endl;
	};

	client.connect("127.0.0.1", port);

	Buffer* buf = new Buffer();

	std::string input;
	while(true){
		std::cin >> input;

		if(input == "QUIT")
			break;

		if(input == "SHUTDOWN"){
			buffer::encode(buf->raw, "SHUTDOWN");
			client.sock.send(buf, client.sock.target);
		}

		else if(input == "PING"){
			buffer::encode(buf->raw, "PING");
			client.sock.send(buf, client.sock.target);
		}

		else{
			buffer::encode(buf->raw, "MSG", input);
			client.sock.send(buf, client.sock.target);
		}

		//Handle Stuff!
		client.handle();
	}

	return 0;
}

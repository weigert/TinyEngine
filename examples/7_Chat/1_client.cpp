#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	std::string username = "Anonymous";
	std::string ipaddress = "127.0.0.1";
	if(!parse::input(argc, args, ipaddress, Tiny::net.port, username)){
		logger::error("Couldn't parse arguments");
		return 0;
	}

	logger::prog = "Chat Server";

	buffer::check();

	Tiny::net.handler["PONG"] = [&](Message m){
		timer::stop();
		int latency = timer::time<std::chrono::microseconds>();
		logger::write("latency", latency, "us");
	};

	Tiny::net.handler["CONNECT"] = [&](Message m){
		std::string name;
		buffer::decode(m.b, name);
		logger::write(name, "connected");
	};

	Tiny::net.handler["MSG"] = [&](Message m){
		std::string name;
		std::string msg;
		buffer::decode(m.b, name, msg);
		std::cout<<"["<<name<<"] "<<msg<<std::endl;
	};

	Buffer* buf = new Buffer();

	//Connect to the server!
	Tiny::net.connect(ipaddress);

	//Connection Request
	buffer::encode(buf, "CONNECT", username);
	Tiny::net.sock.send(buf, Tiny::net.server);

	std::string _input;

	input::start();

	while(true){

		if(input::read(_input)){ //Asynchronous Read
			buf->flush();
			if(_input == "/ping"){
				buffer::encode(buf, "PING");
				Tiny::net.sock.send(buf, Tiny::net.server);
				timer::start();
			}
			else{
				buffer::encode(buf, "MSG", username, _input);
				Tiny::net.sock.send(buf, Tiny::net.server);
			}
		}

		Tiny::net.handle();
	}

	return 0;
}

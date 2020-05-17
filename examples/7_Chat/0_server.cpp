#include "../../TinyEngine.h"

int main( int argc, char* args[] ) {

	//Get the communication port
	if(!parse::input(argc, args, Tiny::net.port)){
		logger::error("Couldn't parse arguments");
		return 0;
	}

	logger::prog = "Chat Server";

	Tiny::net.handler["PING"] = [&](Message m){
		buffer::encode(m.b, "PONG");
		Tiny::net.sock.send(m.b, m.from);
	};

	Tiny::net.handler["CONNECT"] = [&](Message m){
		std::string name;
		buffer::decode(m.b, name);
		logger::write(name, "connected");
		Tiny::net.clients.push_back(m.from);

		//Broadcast new Connection
		m.b->flush();
		buffer::encode(m.b, "CONNECT", name);
		for(auto& c: Tiny::net.clients)
			Tiny::net.sock.send(m.b, c);
	};

	Tiny::net.handler["MSG"] = [&](Message m){
		std::string name, msg;
		buffer::decode(m.b, name, msg);
		std::cout<<"["<<name<<"] "<<msg<<std::endl;
		m.b->flush();
		buffer::encode(m.b, "MSG", name, msg);
		for(auto& c: Tiny::net.clients)
			Tiny::net.sock.send(m.b, c);
	};

	//Launch the Host
	Tiny::net.host();

	Tiny::loop([&](){
		/* Do Nothing */
	});

	Tiny::quit();

	return 0;
}

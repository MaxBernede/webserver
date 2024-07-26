#include "webserver.hpp"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <exception>
#include <thread>
#include <chrono>

int main(int argc, char** argv, char** env) {

	std::list<Server> server;

	try{
		if (argc != 2)
			throw invalidFile();
		std::ifstream conf(argv[1], std::ios::in);
		if (!conf.is_open())
			throw invalidFile();
		server = init_serv(conf, env);
	}
	catch(std::exception const &e){
		Server def(env);
		server.push_front(def);
	}

	// for (Server s : server){
	// 	std::cout << s << std::endl;
	// }

	// initialise ServerRun obj
	ServerRun runningServer(server);
	// Run server loop
	runningServer.serverRunLoop();
	return 0;
}

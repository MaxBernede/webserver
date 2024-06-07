#include<webserver.hpp>
#include<string>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<exception>

//test sleep
#include <thread>
#include <chrono>

// TODO add signal handler to close all fd's when someone presses CTRL C?
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
	for (Server i : server){
		// std::cout << i << std::endl;
	}
	// initialise ServerRun obj
	// ServerRun runningServer(server);
	// Run server loop
	// runningServer.serverRunLoop();
	return 0;
}

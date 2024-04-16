#include "../inc/webserver.hpp"


#include<string>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<exception>

//test sleep
#include <thread>
#include <chrono>


std::string findKey(std::string str){
	if (str.find_first_of("\t\n\v\f\r ") != std::string::npos)
		return str.substr(0, str.find_first_of("\t\n\v\f\r "));
	else
		return str.substr(0, (str.length() - 1));
}

Server	pushBlock(std::list<std::string> block, char **env){
	Server serv(env);
	void (*ptr[9])(std::string, Server&) = {&confPort, &confName, &confRoot, &confMethods, &confCGI,
		&confMaxBody, &confErrorPage, &confIndex, &confAutoIndex};
	std::string const keys[9] = {"listen", "serverName", "root", "allowedMethods", "cgiAllowed",
		"clientMaxBodySize", "errorPage", "index", "autoIndex"};
	bool clear[10] = {false, false, false, false, false,
		false, false, false, false, false};
	for (std::string str : block){
		while (str.find_first_of("\t\n\v\f\r ") == 0)
			str.erase(0, 1);
		if (str.front() == '#')
			continue;
		if (str.back() != ';')
			throw syntaxError();
		try{
			std::string key = findKey(str);
			str.erase(0, key.length());
			while (str.find_first_of("\t\n\v\f\r ") == 0)
				str.erase(0, 1);
			for (int i = 0; i < 10; i++){
				if (key == keys[i]){
					if (clear[i] == false){
						serv.clearData(i);
						clear[i] = true;
					}
					(*ptr[i])(str, serv);
					break;
				}
				if (i == 9)
					throw syntaxError();
			}
		}
		catch(std::exception const &e){
			std::cout << e.what() << std::endl;
			continue;
		}
	}
	return serv;
}

// Loop to create sockets(), bind() and listen() for a server
void serverStartLoop(Server *server)
{
	
} 
 
// TODO add signal handler to close all fd's when someone presses CTRL C
int main(int argc, char** argv, char** env) {

	//!KOEN WORK 
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

	// initialise ServerRun obj
	ServerRun runningServer(server);
	// Run server loop
	runningServer.serverRunLoop();
	return 0;
}

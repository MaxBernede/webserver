#include "../inc/webserver.hpp"


#include<string>
#include<iostream>
#include<iomanip>
#include<fstream>
#include<exception>



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
	for (Server i : server){
		std::cout << i << std::endl;
	}


    //!START OF MY BASE WORK
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    // Prepare the server address structure
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
	int p = genererPort();
    std::cout << "http://localhost:" << p << std::endl;
    server_addr.sin_port = htons(p);

    // Bind the socket to the specified IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen failed" << std::endl;
        return 1;
    }

    while (true) {
        // Accept a new connection
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        Response response(client_fd);
        std::cout << "Client connected" << std::endl;
        // Handle the request from the client
        response.handle_request();

        close(client_fd);
    }

    close(server_fd);
    return 0;
}

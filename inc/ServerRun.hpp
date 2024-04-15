# pragma once

#include <iostream>
#include "webserver.hpp"

// For each server, you need a different listening socket for each port....
class ServerRun {

	private:
		std::list<Server> _servers;
		std::list<Socket> sockets;
		struct epoll events;

	public:
		ServerRun(const Server &config);
		~ServerRun( void );


	void serverRunLoop(Server *server);
	void createSockets(std::vector <s_port> listens);
}

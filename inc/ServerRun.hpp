# pragma once

#include <iostream>
#include "webserver.hpp"

// For each server, you need a different listening socket for each port....
class ServerRun {

	private:
		const Server config;

	public:
		ServerRun(const Server &config);
		~ServerRun( void );


	void serverRunLoop(Server *server);
}

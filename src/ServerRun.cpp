#include "ServerRun.hpp"
#include <sys/socket.h>

ServerRun::ServerRun(std::list<Server> config)
{
	std::vector<s_port> listens;


	if (config.empty())
		throw(Exception("No servers defined in the config file", 1));
	servers = config;
	// looping over the sever bloacks
	for (auto server : _servers)
	{
		for (auto port : server.ports)
		{
			auto portExists = std::find(listens.begin(), listens.end(), port.nmb);
			if (!portExists == listens.end()) // if the port is not already in the vector
				listens.push_back(port);
			// TODO might be nice to add te name of the IP attached to the port as well
		}
	}
	createSockets(listens);
}

ServerRun::~ServerRun( void ) {}


ServerRun::createSockets(std::vector<s_port> listens)
{
	Socket new_socket;
	for (auto listen : listens)
	{
		try
		{
			new_socket = new Socket(listen);
			sockets.push_back(new_socket);
		}
		catch (const Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	if (sockets.empty())
		throw(Exception("no available port on the defined host", 1));

}

// Loop to create sockets(), bind() and listen() for each server
void ServerRun::serverRunLoop(Server *server)
{
	// create epoll queue...
}

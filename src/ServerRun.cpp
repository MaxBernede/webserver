#include "ServerRun.hpp"
#include <sys/socket.h>

ServerRun::ServerRun(const Server &conf) : config(conf)
{
}

ServerRun::~ServerRun( void ) {}


Socket pushSocketListen(uint16_t port_n)
{
	Socket socket_listen (port_n);
	return (socket_listen);
}

// Loop to create sockets(), bind() and listen() for each server
void ServerRun::serverRunLoop(Server *server)
{
	std::list<Socket> sockets;
	std::list<s_port> ports = config.getPorts();

	// creating Socket objects
	for (s_port port: ports)
	{
		uint16_t port_no = port.nmb;
		sockets.push_back(pushSockets(port_no));
	}

	for (Socket s: sockets)
	{
		s.bind();
		s.listen();
	}

	// next up, needs to accept connections
	while (true)
	{
	}

	// once accept_fd is ready, we need to prepare the response

}

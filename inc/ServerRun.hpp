# pragma once

#include <iostream>
#include "webserver.hpp"

// For each server, you need a different listening socket for each port....

enum pollType
{
	LISTENER,
	CLIENT_CONNECTION,
	STATIC_FILE
};

typedef struct t_poll_data
{
	pollType pollType;	
} s_poll_data;

class ServerRun
{
	private:
		std::list<Server> _servers;
		std::vector<Socket *> _listenSockets;
		std::vector<s_poll_data> _pollData;
		std::vector<struct pollfd> _pollFds;

	public:
		ServerRun(const std::list<Server> config);
		~ServerRun( void );

	void serverRunLoop( void );
	void createListenerSockets(std::vector <s_port> listens);
	void addQueue(pollType type, int fd);
	void dataIn(s_poll_data pollData, struct pollfd pollFd, int idx); // read from client
	
	void acceptNewConnection(int listenerFd);
	void readRequest(int clientFd);
	void removeConnection(int idx);

	void dataOut(s_poll_data pollData, struct pollfd pollFd); // write to client


};

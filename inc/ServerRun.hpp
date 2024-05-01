# pragma once

#include <iostream>
#include "webserver.hpp"

#define BUFFER_SIZE 1000

// For each server, you need a different listening socket for each port....
enum pollType
{
	LISTENER,
	CLIENT_CONNECTION,
	STATIC_FILE_READ,
	CGI_READ,
	CGI_WRITE,
	FILE_READ,
	FILE_WRITE
};

// enum pollStatus
// {
// 	CGI_READ,
// 	CGI_WRITE,
// 	FILE_READ,
// 	FILE_WRITE
// };

typedef struct t_poll_data
{
	pollType pollType;
	int serverNum;
} s_poll_data;

class ServerRun
{
	private:
		std::list<Server> _servers;
		std::vector<request> _requests;
		std::vector<Socket *> _listenSockets;
		std::vector<Response> _responses;
		std::vector<s_poll_data> _pollData;
		std::vector<struct pollfd> _pollFds;

	public:
		ServerRun(const std::list<Server> config);
		~ServerRun( void );

	void serverRunLoop( void );
	void createListenerSockets(std::vector<std::pair<int, int> > listens);
	void addQueue(pollType type, int fd, int serverNum);
	
	void acceptNewConnection(int listenerFd, int serverNum);
	void readRequest(int clientFd);
	void removeConnection(int idx);
	void prepareResponse(s_poll_data pollData, int clientFd, int idx);

	void dataIn(s_poll_data pollData, struct pollfd pollFd); // read from client
	void dataOut(s_poll_data pollData, struct pollfd pollFd, int idx); // write to client

	void readFile(int readFd);
	void readPipe(int readFd, s_poll_data pollData);
};

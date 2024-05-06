# pragma once

#include <iostream>
#include "webserver.hpp"
#include <unordered_map>

class CGI;

// For each server, you need a different listening socket for each port....
enum pollType
{
	LISTENER,
	CLIENT_CONNECTION,
	FILE_READ_READING,
	FILE_READ_DONE,
	CGI_READ_WAITING,
	CGI_READ_READING,
	CGI_READ_DONE
};

typedef struct t_poll_data
{
	pollType pollType;
} s_poll_data;


class ServerRun
{
	private:
		std::list<Server> _servers; // Server blocks from config file
		std::vector<Socket *> _listenSockets; // Listener sockets 
		std::unordered_map<int, Request *> _requests; // Request (not cgi)
		std::unordered_map<int, CGI *> _cgi; // Cgi requests, int: pollFd (clientFd) -> create response
		std::unordered_map<int, Response> _responses; // Responses to be sent, int: pollFd to write to
		std::unordered_map<int, s_poll_data> _pollData; // int: polFd
		std::vector<struct pollfd> _pollFds; // vector to pass to poll

	public:
		ServerRun(const std::list<Server> config);
		~ServerRun( void );

	void serverRunLoop( void );
	void createListenerSockets(std::vector<int> listens);
	void addQueue(pollType type, int fd);
	
	void acceptNewConnection(int listenerFd);
	void readRequest(int clientFd);
	void removeConnection(int idx);

	void dataIn(s_poll_data pollData, struct pollfd pollFd); // read from client
	void dataOut(s_poll_data pollData, struct pollfd pollFd, int idx); // write to client

	void readFile(int readFd);
	void readPipe(int readFd, s_poll_data pollData);
};

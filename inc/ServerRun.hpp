# pragma once

#include <iostream>
#include "webserver.hpp"

class CGI;
class HTTPObject;

// For each server, you need a different listening socket for each port....
enum pollType
{
	LISTENER,
	CLIENT_CONNECTION_READY,
	CLIENT_CONNECTION_WAIT,
	REQUEST_READING,
	REQUEST_READ_DONE,
	FILE_READ_READING,
	FILE_READ_DONE,
	CGI_READ_WAITING,
	CGI_READ_READING,
	CGI_READ_DONE,
	SEND_REDIR,
	HTTP_REDIRECT
};

typedef struct t_poll_data
{
	pollType _pollType;
} s_poll_data;


class ServerRun
{
	private:
		std::list<Server> _servers; // Server blocks from config file
		std::vector<Socket *> _listenSockets; // Listener sockets 
		std::unordered_map<int, HTTPObject *> _httpObjects;
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
	void removeConnection(int fd);

	void dataIn(s_poll_data pollData, struct pollfd pollFd); // read from client
	void dataOut(s_poll_data pollData, struct pollfd pollFd); // write to client

	void handleCGIRequest(int clientFd);
	void handleStaticFileRequest(int clientFd);
	void redirectToError(int ErrCode, int clientFd); // Redirect to 404, 405
	
	void readFile(int fd);
	void readPipe(int fd);
	void sendResponse(int fd);
	void sendRedirect(int fd);
	void sendError(int fd);

	HTTPObject *findHTTPObject(int readFd);
	void		cleanUp(int clientFd);
	Server 		getConfig(int port);
	Server 		getConfig(std::string host);
};

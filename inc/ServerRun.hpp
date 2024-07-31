#pragma once

#include "Webserver.hpp"
#include "Socket.hpp"
#include <unordered_map>
#include <sys/poll.h>
#include <algorithm>


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
	HTTP_ERROR,
	HTTP_REDIRECT,
	AUTO_INDEX,
	EMPTY_RESPONSE
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
	
	void	acceptNewConnection(int listenerFd);
	void	handleRequest(int clientFd);
	void	executeRequest(int clientFd, Server config);
	void	removeConnection(int fd);

	void dataIn(s_poll_data pollData, struct pollfd pollFd); // read from client
	void dataOut(s_poll_data pollData, struct pollfd pollFd); // write to client

	void	handleCGIRequest(int clientFd);
	void 	handleStaticFileRequest(int clientFd);
	void	DirectoryListing(int clientFd);
	void	redirectToError(ErrorCode ErrCode, int clientFd); // Redirect to 404, 405
	int 	httpRedirect(ErrorCode status, int clientfd);
	void	handleHTTPError(ErrorCode err, int fd);

	void readFile(int fd);
	void readPipe(int fd);
	void sendResponse(int fd);
	void sendAutoIndex(int cleintFd);
	void sendRedirect(int clientFd);
	void sendError(int clietnFd);

	Server findConfig(s_domain port);

	HTTPObject *findHTTPObject(int readFd);
	void		cleanUp(int clientFd);
};

#pragma once

#include "Webserver.hpp"
#include "Socket.hpp"
#include <unordered_map>
#include <sys/poll.h>
#include <algorithm>

class CGI;
class HTTPObject;

// For each server, you need a different listening socket for each port....
enum pollState
{
	LISTENER,
	CLIENT_CONNECTION_READY,
	CLIENT_CONNECTION_WAIT, // waiting for reading a file or pipe
	REQUEST_READING,
	REQUEST_READ_DONE,
	FILE_READ_READING,
	FILE_READ_DONE,
	CGI_WRITE_TO_PIPE, // post body to the cgi process
	CGI_WRITE_STOP,
	CGI_READ_WAITING, // waiting for cgi to finish running child process
	CGI_READ_READING, // reading cgi pipe to prepare response
	CGI_READ_DONE, // response ready to send to client
	HTTP_ERROR,
	HTTP_REDIRECT,
	EMPTY_RESPONSE
};

enum fdType
{
	SOCKET,
	CLIENTFD,
	READFD,
	WRITEFD
};

typedef struct t_poll_data
{
	pollState _pollState;
	fdType	_fdType;
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

		void	serverRunLoop( void );
		void	createListenerSockets(std::vector<int> listens);
		void	addQueue(pollState state, fdType type, int fd);

		void	acceptNewConnection(int listenerFd);
		void	handleRequest(int clientFd);
		void	executeRequest(int clientFd, Server config);
		void	removeConnection(int fd);

		void	dataIn(s_poll_data pollData, struct pollfd pollFd); // read from client
		void	dataOut(s_poll_data pollData, struct pollfd pollFd); // write to client

		void	uploadToCgi(int writePipe);
		void	handleCgiRequest(int clientFd);

		void 	handleStaticFileRequest(int clientFd);

		void	redirectToError(ErrorCode ErrCode, int clientFd); // Redirect to 404, 405
		int 	httpRedirect(ErrorCode status, int clientfd);
		void	handleHTTPError(ErrorCode err, int fd);

		void	readFile(int fd);
		void	readPipe(int fd);

		// Send functions
		void	sendResponse(int readFd);
		void	sendCgiResponse(int pipeFd);
		void	sendRedirect(int clientFd);
		void	sendError(int clietnFd);

		Server	findConfig(s_domain port);

		void	cleanUp(int clientFd);
		HTTPObject	*findHTTPObject(int fd);
};

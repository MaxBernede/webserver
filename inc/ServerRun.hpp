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
<<<<<<< HEAD
	int pollType;
=======
	pollType pollType;
	int serverNum;
>>>>>>> sock_class
} s_poll_data;

class ServerRun
{
	private:
<<<<<<< HEAD
		//List of all the servers available from the config file
		std::list<Server> _servers;
		
		//All the sockets open
		std::vector<Socket *> _listenSockets;
		std::vector<s_poll_data> _pollData;
		
=======
		std::list<Server> _servers;
		std::vector<Socket *> _listenSockets;
		std::vector<s_poll_data> _pollData;
>>>>>>> sock_class
		std::vector<struct pollfd> _pollFds;

	public:
		ServerRun(const std::list<Server> config);
		~ServerRun( void );

	void serverRunLoop( void );
<<<<<<< HEAD
	void createListenerSockets(std::vector <int> listens);
	void addQueue(pollType type, int fd);
	void dataIn(s_poll_data pollData, struct pollfd pollFd, int idx); // read from client
	
	void acceptNewConnection(int listenerFd);
	void readRequest(int clientFd);
	void removeConnection(int idx);

	void dataOut(s_poll_data pollData, struct pollfd pollFd); // write to client


=======
	void createListenerSockets(std::vector<std::pair<int, int>> listens);
	void addQueue(pollType type, int fd, int serverNum);
	
	void acceptNewConnection(int listenerFd, int serverNum);
	void readRequest(int clientFd);
	void removeConnection(int idx);

	void dataIn(s_poll_data pollData, struct pollfd pollFd, int idx); // read from client
	void dataOut(s_poll_data pollData, struct pollfd pollFd); // write to client
>>>>>>> sock_class
};

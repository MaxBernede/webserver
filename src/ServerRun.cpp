#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <sys/socket.h>

ServerRun::ServerRun(const std::list<Server> config)
{
	// std::vector<int> listens;
	// pairing fd and server number
	std::vector<std::pair<int, int>> listens;
	int serverNum = 0;


	if (config.empty())
		throw(Exception("No servers defined in the config file", 1));
	_servers = config;
	// looping over the sever bloacks
	for (auto server : _servers)
	{
		for (auto port : server.getPorts())
		{
			std::pair<int, int> new_pair;
			new_pair.first = port.nmb;
			new_pair.second = serverNum;
			listens.push_back(new_pair);
			// TODO might be nice to add te name of the IP attached to the port as well
		}
		serverNum++;
	}
	//create listening sockets
	createListenerSockets(listens);
}

ServerRun::~ServerRun( void )
{
	// delete everything
	for (auto it : _listenSockets)
	{
		close(it->getFd());
		delete it;
	}
}

void ServerRun::createListenerSockets(std::vector<std::pair<int, int> > listens)
{
	Socket *new_socket;
	for (auto listen : listens)
	{
		try
		{
			new_socket = new Socket(listen.first, listen.second);
			_listenSockets.push_back(new_socket);
		}
		catch (const Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	if (_listenSockets.empty())
		throw(Exception("No available port on the defined host", 1));
	// add listener sockets to queue
	for (int i = 0; i < (int)_listenSockets.size(); i++)
	{
		addQueue(LISTENER, _listenSockets[i]->getFd(), _listenSockets[i]->getServerNum());
	}
}

void ServerRun::addQueue(pollType type, int fd, int serverNum)
{
	s_poll_data newPollItem;
	struct pollfd newPollFd;

	newPollFd = {fd, POLLIN | POLLOUT, 0};
	newPollItem.serverNum = serverNum;
	newPollItem.pollType = type;
	_pollFds.push_back(newPollFd);
	_pollData.push_back(newPollItem);
}

// Loop to create sockets(), bind() and listen() for each server
void ServerRun::serverRunLoop( void )
{
	// create epoll queue...
	int nCliCon = -1;

	std::cout << "Entered run loop function" << std::endl;
	while (true)
	{
		
		nCliCon = poll(_pollFds.data(), _pollFds.size(), 0);
		if (nCliCon <= 0)
		{
			if (nCliCon < 0 and errno != EAGAIN) // EGAIN: Resource temporarily unavailable
				throw(Exception("Poll failed", errno));
			continue ;
		}
		for (int i = 0; i < (int)_pollFds.size(); i++)
		{
			try
			{
				if (_pollFds[i].revents & POLLIN)
				{
					//Read from client
					dataIn(_pollData[i], _pollFds[i]);
				}
				if (_pollFds[i].revents & POLLOUT)
				{
					// Write to client
					dataOut(_pollData[i], _pollFds[i], i);
				}
			}
			catch(const Exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
}

void ServerRun::acceptNewConnection(int listenerFd, int serverNum)
{
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	if (connFd == -1)
		throw(Exception("accept() errored and returned -1", errno));
	
	addQueue(CLIENT_CONNECTION, connFd, serverNum);
}

void ServerRun::readRequest(int clientFd)
{
	request request(clientFd);
	_requests.push_back(request);
}

void ServerRun::removeConnection(int idx)
{
	_pollData.erase(_pollData.begin() + idx);
	_pollFds.erase(_pollFds.begin() + idx);
}

void ServerRun::readFile(int readFd)
{
	
}

// readFd should match one in CGI
void ServerRun::readPipe(int readFd, s_poll_data pollData)
{
	Response newResponse(_cgi[readFd].getClientFd());
	
	newResponse.read_contents();
	pollData.pollType = CGI_WRITE;
}


void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{
	switch (pollData.pollType)
	{
		case LISTENER:
			acceptNewConnection(pollFd.fd, pollData.serverNum);
			break ;
		case CLIENT_CONNECTION: 
			readRequest(pollFd.fd); // TODO this READS and WRITES the request and closes connection... we need to separate this 
			break ;
		// Add case to read the request here -> add file to read for request in poll_fd as STATIC_FILE
		case CGI_READ:
			// Read from the CGI pipe
			readPipe(pollFd.fd, pollData);
			break ;
		case FILE_READ: // Read a static html file
			break ;
		default:
			break ;
	}
}

void ServerRun::prepareResponse(s_poll_data pollData, int clientFd, int idx)
{
	//find the request based on clientFd
	// using an unordered map is better...
	int i = 0;
	if (!_requests.size())
		return ;
	while (i < (int)_requests.size())
	{
		if (_requests[i].getClientFd() == clientFd)
			break ;
		i++;
	}
	if (_requests[i].isCgi()) // TODO: && CGI is allowed
	{
		CGI newCgi(_requests[i], clientFd);
		newCgi.runCgi();
		_cgi[newCgi.getReadFd()] = newCgi; // when a new CGI object is created, add it to the map
		addQueue(CGI_READ, newCgi.getReadFd(), pollData.serverNum);
	}
	else
	{
		Response newResponse(_requests[i], clientFd);
		newResponse.read_contents();
		// remove request?
		
		
		close(clientFd);
		_requests.erase(_requests.begin() + i);
		removeConnection(idx);
	}
}

void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd, int idx)
{
	switch (pollData.pollType)
	{
		case CLIENT_CONNECTION:
			prepareResponse(pollData, pollFd.fd, idx); // write to the client
			break ;
		case CGI_WRITE:
			break ;
		case FILE_WRITE:
			break ;
		default:
			break ;
	}
}

/*

vector requests
vector responses (response string)
vector cgi

CLIENT_CONNECTION (write here) --> read to see the request / write to response
CGI_PIPE_READ (read html file) --> to create the response string
FILE_READ (read html file) --> to create the response string
FILE_WRITE (needed for post method?)
LISTENER_SOCKET (read to see new connection requests)

*/
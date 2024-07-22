#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "CGI.hpp"

ServerRun::ServerRun(const std::list<Server> config)
{
	std::vector<int> listens;

	if (config.empty())
		throw(Exception("No servers defined in the config file", 1));
	_servers = config;
	// looping over the sever bloacks
	for (auto server : _servers)
	{
		for (auto port : server.getPorts())
		{
			if (std::find(listens.begin(), listens.end(), port.port) == listens.end())
				listens.push_back(port.port);
			else
			{
				Logger::log("Servers have the same port in config: " + std::to_string(port.port), LogLevel::WARNING);
			}
		}
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

void ServerRun::createListenerSockets(std::vector<int> listens)
{
	Socket *new_socket;
	std::cout << "Creaing listening sockets\n";
	for (auto listen : listens)
	{
		try
		{
			new_socket = new Socket(listen);
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
		addQueue(LISTENER, _listenSockets[i]->getFd());
	}
}

void ServerRun::addQueue(pollType type, int fd)
{
	s_poll_data newPollItem;
	struct pollfd newPollFd;

	//Logger::log("Type" + std::to_string(type) + std::to_string(fd), WARNING);
	newPollFd = {fd, POLLIN | POLLOUT, 0};
	newPollItem._pollType = type;
	_pollFds.push_back(newPollFd);
	_pollData[fd] = newPollItem;
}

void ServerRun::serverRunLoop( void )
{
	int nCon = -1;
	Logger::log("Server running... ", INFO);
	while (true)
	{
		nCon = poll(_pollFds.data(), _pollFds.size(), 0);
		if (nCon <= 0)
		{
			if (nCon < 0 and errno != EAGAIN) // EGAIN: Resource temporarily unavailable
				throw(Exception("Poll failed", errno));
			continue ;
		}
		for (int i = 0; i < (int)_pollFds.size(); i++)
		{
			int fd = _pollFds[i].fd;
			try
			{
				if (_pollFds[i].revents & POLLIN)
				{
					// Only start reading CGI once the write end of the pipe is closed
					if ((_pollFds[i].revents & POLLHUP) && _pollData[fd]._pollType == CGI_READ_WAITING)
					{
						std::cout << "CGI write side finished writing to the pipe\n";
						_pollData[fd]._pollType = CGI_READ_READING;
					}
					//Read from client
					dataIn(_pollData[fd], _pollFds[i]);
				}
				if (_pollFds[i].revents & POLLOUT || _pollData[fd]._pollType == CGI_READ_DONE)
				{
					// Write to client
					dataOut(_pollData[fd], _pollFds[i]);
					//Logger::log("data out finished", ERROR);
				}

			}
			catch(const Exception& e)
			{
				//Cath of the "Throw Port not found" in the readRequest;
				//std::cerr << e.what() << '\n';
				;
			}
		}
	}
}

/// TODO check functions with same name
Server ServerRun::getConfig(int port) // WILL ADD HOST
{
	for (auto server : _servers)
	{
		for (auto p : server.getPorts())
		{
			if (p.port == port)
			{
				return (server);
			}
		}
	}
	throw(Exception("Server not found", 1));
}

Server ServerRun::getConfig(s_domain port){
	for (auto server : _servers){
		for (auto p : server.getPorts()){
			if (p.port == port.port && p.host == port.host){
				return (server);
			}
		}
	}
	throw (Exception("Server not found", 404));
	return (nullptr);
}

void ServerRun::removeConnection(int fd)
{
	for (int i = 0; i < (int)_pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break ;
		}
	}
	if (_pollData.count(fd))
		_pollData.erase(fd);
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

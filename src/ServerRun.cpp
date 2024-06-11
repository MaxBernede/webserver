#include "webserver.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <sys/socket.h>
#include "CGI.hpp"

// TODO: remove 
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
				std::cout << "port: " << port.port << std::endl;
				std::cout << "Servers have the same port in config" << std::endl;
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

	newPollFd = {fd, POLLIN | POLLOUT, 0};
	newPollItem._pollType = type;
	_pollFds.push_back(newPollFd);
	_pollData[fd] = newPollItem;
}

// Loop to create sockets(), bind() and listen() for each server
void ServerRun::serverRunLoop( void )
{
	// create epoll queue...
	int nCon = -1;
	printColor(GREEN, "Server running...");
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
				}
			}
			catch(const Exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}
}

void ServerRun::acceptNewConnection(int listenerFd)
{
	int connFd = -1;
	struct sockaddr_in *cli_addr = {};
	socklen_t len = sizeof(sockaddr_in);

	connFd = accept(listenerFd, (struct sockaddr *)cli_addr, &len);
	std::cout << "New Client Connected accepted\n";
	if (connFd == -1)
		throw(Exception("accept() errored and returned -1", errno));
	addQueue(CLIENT_CONNECTION_READY, connFd);
}

Server ServerRun::getConfig(int port)
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
	return (nullptr);
}

Server ServerRun::getConfig(std::string host)
{
	for (auto server : _servers)
	{
		if (server.getName() == host)
		{
			return (server);
		}
	}
	throw Exception("Server not found", 404);
	return (nullptr);
}

// Only continue after reading the whole request
void ServerRun::readRequest(int clientFd)
{	
	if (_requests.find(clientFd) == _requests.end())
	{
		Request *newRequest = new Request(clientFd);
		_requests[clientFd] = newRequest;
	}
	else if (_requests[clientFd]->isDoneReading() == false)
	{
		_requests[clientFd]->readRequest();
	}
	if (_requests[clientFd]->isDoneReading() == true)
	{
		int port = _requests[clientFd]->getRequestPort();
		if (port < 0)
			throw Exception("Port not found", errno);
		Server config = getConfig(port);
		//TODO if server == not found, error should be thrown, please catch
		_requests[clientFd]->setConfig(config);
		_requests[clientFd]->checkRequest();
		std::cout << "ROOT directory:\t"
			<< _requests[clientFd]->getConfig().getRoot() << std::endl;
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_WAIT;
		// TODO check if _requests[clientFd]->getFileName() is defined in the configs redirect
		if (_requests[clientFd]->isRedirect()){
			std::cout << "I don't know how any of this works, man" << std::endl;
		}
		else {
			if (_requests[clientFd]->isCgi()) {
				if (!config.getCGI())
				{
					std::cout << "CGI is not allowed for this server\n"; // TODO: send some sort of error
					return ;
				}
				std::cout << "It is a CGI Request!\n";
				CGI *cgiRequest = new CGI(_requests[clientFd], clientFd);
				int pipeFd = cgiRequest->getReadFd();
				_cgi[pipeFd] = cgiRequest;
				// std::cout << "Cgi Pipe FD: " << pipeFd << std::endl;
				addQueue(CGI_READ_WAITING, pipeFd);
				cgiRequest->runCgi();
			}
			else { // Static file
				std::string filePath = _requests[clientFd]->getConfig().getRoot() + _requests[clientFd]->getFileName();
				std::cout << "opening file: " << filePath << std::endl;
				int fileFd = open(filePath.c_str(), O_RDONLY);
				if (fileFd < 0)
				{
					std::cout << "Failed opening file: " << filePath << std::endl; // TODO 404 error
					throw(Exception("Opening static file failed", errno));
				}
				_requests[fileFd] = _requests[clientFd]; // TODO: We need to add the request header reading in here too
				addQueue(FILE_READ_READING, fileFd);
			}
		}
		std::string body = _requests[clientFd]->getValues("Body");
		// std::cout << _requests[clientFd]->getConfig() << std::endl;
		if (!body.empty()){
			printColor(RED, "BODY CREATE");
			create_file(body, _requests[clientFd]->getConfig().getRoot());
		}
		_requests.erase(clientFd);
	}
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

void ServerRun::readFile(int fd) // Static file fd
{
	char buffer[BUFFER_SIZE];

	memset(buffer, '\0', BUFFER_SIZE);
	int clientFd = _requests[fd]->getClientFd();
	if (_responses.find(clientFd) == _responses.end()) // Response object not created
	{
		Response *response = new Response(_requests[fd], clientFd);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	if (readChars < 0)
		throw(Exception("Read file failed", errno));
	if (readChars > 0)
	{
		_responses[clientFd]->addToBuffer(buffer);
	}
	if (readChars == 0)
	{
		_pollData[fd]._pollType = FILE_READ_DONE;
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::readPipe(int fd) // Pipe read end fd
{
	char buffer[BUFFER_SIZE];

	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = '\0';
	int clientFd = _cgi[fd]->getClientFd();
	if (_responses.find(clientFd) == _responses.end()) // Response object not created
	{
		Response *response = new Response(_cgi[fd]->getRequest(), clientFd);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE - 1);
	std::cout << "Cgi read chars " << readChars << std::endl;
	if (readChars < 0)
		throw(Exception("Read pipe failed!", errno));
	if (readChars > 0)
	{
		std::cout << "Cgi: adding to buffer\n";
		_responses[clientFd]->addToBuffer(buffer);
	}
	if (readChars < BUFFER_SIZE - 1)
	{
		_pollData[fd]._pollType = CGI_READ_DONE;
		std::cout << "Setting CGI READ DONE\n";
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{
	// std::cout << "Poll FD: " << pollFd.fd << " Poll type: " << pollData.pollType << std::endl;
	switch (pollData._pollType)
	{
		case LISTENER:
			acceptNewConnection(pollFd.fd);
			break ;
		case CLIENT_CONNECTION_READY: 
			readRequest(pollFd.fd);
			break ;
		case CGI_READ_READING:
			readPipe(pollFd.fd);
			break ;
		case FILE_READ_READING:
			readFile(pollFd.fd);
			break ;
		default:
			break ;
	}
}

void ServerRun::sendResponse(int fd)
{
		std::cout << "SENDING RESPONSE" << std::endl;
		int clientFd = _requests[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		removeConnection(fd);
		// _requests.erase(clientFd);
		if (_responses.count(clientFd) == 1)
		{
			std::cout << "1\n";
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(fd) == 1)
		{
			std::cout << "2\n";
			delete _requests[fd];
			_requests.erase(fd);
		}
		if (!_responses.count(clientFd) and !_requests.count(fd))
		{
			std::cout << "3\n";
			close(clientFd); // only loads in the browser one the fd is closed...should we keep the connection?
			removeConnection(clientFd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendCgiResponse(int fd)
{
		std::cout << "SENDING CGI RESPONSE" << std::endl;
		int clientFd = _cgi[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connectioned?
		removeConnection(fd);
		if (_cgi.count(fd))
		{
			delete _cgi[fd];
			_cgi.erase(fd);
		}
		if (_responses.count(clientFd))
		{
			delete _responses[clientFd];
			_responses.erase(clientFd);
		}
		if (_requests.count(fd))
		{
			delete _requests[fd];
			_requests.erase(fd);
		}
		_pollData[clientFd]._pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd)
{
	switch (pollData._pollType)
	{
		case CGI_READ_DONE:
			sendCgiResponse(pollFd.fd);
			break ;
		case FILE_READ_DONE:
			sendResponse(pollFd.fd);
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
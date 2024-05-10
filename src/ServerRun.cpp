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
			listens.push_back(port.nmb);
			// TODO might be nice to add te name of the IP attached to the port as well
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
	newPollItem.pollType = type;
	_pollFds.push_back(newPollFd);
	_pollData[fd] = newPollItem;
}

// Loop to create sockets(), bind() and listen() for each server
void ServerRun::serverRunLoop( void )
{
	// create epoll queue...
	int nCliCon = -1;

	printColor(GREEN, "Server running...");
	for (auto item : _pollFds)
	{
		std::cout << item.fd << std::endl;
	}
	while (true)
	{
		
		nCliCon = poll(_pollFds.data(), _pollFds.size(), 0);
		// if (nCliCon)
		// 	std::cout << "ncliCon: " << nCliCon << std::endl;
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
					// Only start reading CGI once the write end of the pipe is closed
					if ((_pollFds[i].revents & POLLHUP) && _pollData[_pollFds[i].fd].pollType == CGI_READ_WAITING)
					{
						std::cout << "CGI write side finished writing to the pipe\n";
						_pollData[_pollFds[i].fd].pollType = CGI_READ_READING;
					}
					if (_pollData[_pollFds[i].fd].pollType == CGI_READ_READING)
						std::cout <<  "cgi read waiting!\n";
					//Read from client
					dataIn(_pollData[_pollFds[i].fd], _pollFds[i]);
				}
				if (_pollFds[i].revents & POLLOUT || _pollData[_pollFds[i].fd].pollType == CGI_READ_DONE)
				{
					// Write to client
					dataOut(_pollData[_pollFds[i].fd], _pollFds[i]);
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
	if (connFd == -1)
		throw(Exception("accept() errored and returned -1", errno));
	addQueue(CLIENT_CONNECTION_READY, connFd);
}

void ServerRun::readRequest(int clientFd)
{
	Request *newRequest = new Request(clientFd);

	_pollData[clientFd].pollType = CLIENT_CONNECTION_WAIT;
	if (newRequest->isCgi())
	{
		std::cout << "It is a CGI Request!\n";
		CGI *cgiRequest = new CGI(newRequest, clientFd);
		int pipeFd = cgiRequest->getReadFd();
		_cgi[pipeFd] = cgiRequest;
		std::cout << "PIPE FD: " << pipeFd << std::endl;
		addQueue(CGI_READ_WAITING, pipeFd);
		cgiRequest->runCgi();
	}
	else // Static file
	{
		std::string filePath = "html/" + newRequest->getFileName();
		int fileFd = open(filePath.c_str(), O_RDONLY);
		if (fileFd < 0)
			throw(Exception("Opening static file failed!", errno));
		_requests[fileFd] = newRequest; // TODO: We need to add the request header reading in here too
		addQueue(FILE_READ_READING, fileFd);
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
	_pollData.erase(fd);
}

void ServerRun::readFile(int fd) // Static file fd
{
	char buffer[BUFFER_SIZE];

	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = '\0';
	int clientFd = _requests[fd]->getClientFd();
	if (_responses.find(clientFd) == _responses.end()) // Response object no created
	{
		Response *response = new Response(_requests[fd], clientFd);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE);
	if (readChars < 0)
		throw(Exception("Read file failed", errno));
	if (readChars > 0)
	{
		_responses[clientFd]->addToBuffer(buffer);
	}
	if (readChars == 0)
	{
		_pollData[fd].pollType = FILE_READ_DONE;
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::readPipe(int fd) // Pipe read end fd
{
	char buffer[BUFFER_SIZE];

	std::cout << "Fd reading from pipe: " << fd << std::endl;
	for (int i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = '\0';
	int clientFd = _cgi[fd]->getClientFd();
	if (_responses.find(clientFd) == _responses.end()) // Response object not created
	{
		Response *response = new Response(_cgi[fd]->getRequest(), clientFd);
		_responses[clientFd] = response;
	}
	int readChars = read(fd, buffer, BUFFER_SIZE);
	std::cout << "cgi read chars " << readChars << std::endl;
	if (readChars < 0)
		throw(Exception("Read pipe failed!", errno));
	if (readChars > 0)
	{
		std::cout << "adding it!!!\n";
		_responses[clientFd]->addToBuffer(buffer);
	}
	if (readChars == 0)
	{
		_pollData[fd].pollType = CGI_READ_DONE;
		std::cout << "Setting CGI READ DONE\n";
		_responses[clientFd]->setReady();
		close(fd);
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd)
{
	switch (pollData.pollType)
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
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connectioned?
		removeConnection(fd);
		_requests.erase(fd);
		_responses.erase(clientFd);
		_pollData[clientFd].pollType = CLIENT_CONNECTION_READY;
}

void ServerRun::sendCgiResponse(int fd)
{
		std::cout << "SENDING RESPONSE" << std::endl;
		int clientFd = _cgi[fd]->getClientFd();
		Response *r = _responses[clientFd];
		r->rSend();
		close(clientFd); // only loads in the browser one the fd is closed...should we keep the connectioned?
		removeConnection(fd);
		_cgi.erase(fd);
		_responses.erase(clientFd);
}

void ServerRun::dataOut(s_poll_data pollData, struct pollfd pollFd)
{
	switch (pollData.pollType)
	{
		case CGI_READ_DONE:
			std::cout << "TRYING TO SEND CGI RESPONSE\n";
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
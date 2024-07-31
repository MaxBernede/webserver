#include "ServerRun.hpp"

#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

ServerRun::ServerRun(const std::list<Server> config)
{
	std::vector<int> listens;

	if (config.empty())
		throw (Exception("No servers defined in the config file", 1));
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
	
	for (int i = 0; i < (int)_listenSockets.size(); i++) {
		addQueue(LISTENER, SOCKET, _listenSockets[i]->getFd()); // add listener sockets to queue
	}
}

void ServerRun::addQueue(pollState state, fdType type, int fd)
{
	s_poll_data newPollItem;
	struct pollfd newPollFd;

	newPollFd = {fd, POLLIN | POLLOUT, 0};
	newPollItem._pollState = state;
	newPollItem._fdType = type;
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

		if (nCon < 0 and errno != EAGAIN) // EGAIN: Resource temporarily unavailable
			throw (Exception("Poll failed", errno));

		for (int i = 0; i < (int)_pollFds.size(); i++)
		{
			int fd = _pollFds[i].fd;
			try
			{
				HTTPObject *obj = findHTTPObject(fd);
				if (obj != nullptr)
					obj->checkTimeOut();
				if (_pollFds[i].revents & POLLIN)
				{
					// Only start reading CGI once the write end of the pipe is closed
					if ( _pollData[fd]._pollState == CGI_READ_WAITING)
					{
						if (_pollFds[i].revents & POLLHUP && _pollData[fd]._pollState == CGI_READ_WAITING)
						{
							// Logger::log("CGI did not TimedOut");
							_pollData[fd]._pollState = CGI_READ_READING;
						}
					}
					dataIn(_pollData[fd], _pollFds[i]);						//Read from client
				}

				if (_pollFds[i].revents & POLLOUT || _pollData[fd]._pollState == CGI_READ_DONE){
					dataOut(_pollData[fd], _pollFds[i]);					// Write to client
				}
			}
			catch(const HTTPError& e)
			{ 
				ErrorCode err = e.getErrorCode();
				Logger::log(e.what(), LogLevel::ERROR);
				if (_pollData[fd]._fdType == CLIENTFD)
					_httpObjects[fd]->_request->setErrorCode(err);
				else
					findHTTPObject(fd)->_request->setErrorCode(err);
				_pollData[fd]._pollState = CLIENT_CONNECTION_WAIT;
				handleHTTPError(err, fd);
			}
			// Do we have uncaught exception?
		}
	}
}

void ServerRun::handleHTTPError(ErrorCode err, int fd){
	if (err >= MULTIPLE_CHOICE && err <= PERM_REDIR)
	{
		int ErrCode = httpRedirect(err, fd);
		if (ErrCode == err)
			_pollData[fd]._pollState = HTTP_REDIRECT;

		_httpObjects[fd]->_request->setErrorCode(ErrorCode(ErrCode));
	}
	if (err < MULTIPLE_CHOICE || err > PERM_REDIR)
		redirectToError(err, fd);
}


//return server or throw Exception
Server ServerRun::findConfig(s_domain port){
	for (Server server : _servers){
		std::string name = server.getName();
		for (s_domain p : server.getPorts()){
			if (p.port == port.port && (p.host == port.host || name == port.host)){
				return (server);
			}
		}
	}
	throw (HTTPError(INTERNAL_SRV_ERR));
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

HTTPObject *ServerRun::findHTTPObject(int fd)
{
	for (auto& pair : _httpObjects)
	{
		if (pair.second->getReadFd() == fd) {
		    return pair.second;
		}
	}
	for (auto& pair : _httpObjects)
	{
		if (pair.second->getWriteFd() == fd) {
		    return pair.second;
		}
	}
	return nullptr; // Return nullptr if not found
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

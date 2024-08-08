#include "ServerRun.hpp"

#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>

std::vector<s_domain>::iterator findDomain(std::vector<s_domain>::iterator start,
	std::vector<s_domain>::iterator end, s_domain domain)
{
	while (start != end && (*start).port != domain.port)
		start++;
	return start;
}

ServerRun::ServerRun(const std::list<Server> config)
{

	if (config.empty())
		throw (Exception("No servers defined in the config file", 1));
	_servers = config;
	// looping over the sever bloacks
	for (Server server : _servers)
	{
		std::vector<s_domain> listens;
		for (s_domain domain : server.getPorts())
		{
			if (findDomain(listens.begin(), listens.end(), domain) == listens.end())
				listens.push_back(domain);
			else
				Logger::log("Servers have the same port in config: " + std::to_string(domain.port), LogLevel::WARNING);
		}
		//create listening sockets
		createListenerSockets(listens, server.getName());
	}
}

ServerRun::~ServerRun( void )
{
	// delete everything
	for (Socket	*it : _listenSockets)
	{
		close(it->getFd());
		delete it;
	}
}

void ServerRun::createListenerSockets(std::vector<s_domain> listens, std::string name)
{
	Socket *new_socket;
	Logger::log("Creaing listening sockets\n", LogLevel::INFO);
	for (s_domain listen : listens)
	{
		try
		{
			new_socket = new Socket(listen);
			_listenSockets.push_back(new_socket);
		} catch (const Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	try
	{
		new_socket = new Socket(name);
		_listenSockets.push_back(new_socket);
	} catch (const Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	if (_listenSockets.empty())
		throw(Exception("No available port on the defined host", 1));
	
	for (int i = 0; i < (int)_listenSockets.size(); i++)
		addQueue(LISTENER, SOCKET, _listenSockets[i]->getFd()); // add listener sockets to queue
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

void ServerRun::serverRunLoop(void)
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
				if (obj != nullptr && !obj->getTimeOut())
					obj->checkTimeOut();
				if (_pollFds[i].revents & POLLIN)
				{
					// Only start reading CGI once the write end of the pipe is closed
					if (_pollFds[i].revents & POLLHUP && _pollData[fd]._pollState == CGI_READ_WAITING)
					{
						if (obj != nullptr && !obj->getTimeOut())
							_pollData[fd]._pollState = CGI_READ_READING;
					}
					dataIn(_pollData[fd], _pollFds[i]);						//Read from client
				}
				if (_pollFds[i].revents & POLLOUT || _pollData[fd]._pollState == CGI_READ_DONE)
					dataOut(_pollData[fd], _pollFds[i]);					// Write to client
			}
			catch(const Exception& e)
			{
				Logger::log(e.what(), LogLevel::ERROR);
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
		}
	}
}

void ServerRun::handleHTTPError(ErrorCode err, int fd)
{
	if (err == DIRECTORY_LISTING)
	{
		DirectoryListing(fd);
		_pollData[fd]._pollState = AUTO_INDEX;
	}
	else if (err >= MULTIPLE_CHOICE && err <= PERM_REDIR)
	{
		int ErrCode = httpRedirect(err, fd);
		if (ErrCode == err)
			_pollData[fd]._pollState = HTTP_REDIRECT;

		_httpObjects[fd]->_request->setErrorCode(ErrorCode(ErrCode));
	}
	else if (err < MULTIPLE_CHOICE || err > PERM_REDIR)
		redirectToError(err, fd);
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
		if (pair.second->getReadFd() == fd)
			return pair.second;
	}
	for (auto& pair : _httpObjects)
	{
		if (pair.second->getWriteFd() == fd)
			return pair.second;
	}
	return nullptr; // Return nullptr if not found
}
#include "ServerRun.hpp"
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

void ServerRun::createListenerSockets(std::vector<std::pair<int, int>> listens)
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
	for (int i = 0; i < _listenSockets.size(); i++)
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
		for (int i = 0; i < _pollFds.size(); i++)
		{
			try
			{
				if (_pollFds[i].revents & POLLIN)
				{
					//Read from client
					dataIn(_pollData[i], _pollFds[i], i);
				}
				if (_pollFds[i].revents & POLLOUT)
				{
					// Write to client
					// currently writing is not going through poll... NEEDS A FIX
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
	Response response(clientFd);
	response.handle_request();
	close(clientFd);
}

void ServerRun::removeConnection(int idx)
{
	int i = 0;
	for (auto itData = _pollData.begin(); itData != _pollData.end(); itData++)
	{
		if (i == idx)
		{
			_pollData.erase(itData);
			break ;
		}
		i++;
	}

	i = 0;
	for (auto itFd = _pollFds.begin(); itFd != _pollFds.end(); itFd++)
	{
		if (i == idx)
		{
			_pollFds.erase(itFd);
			break ;
		}
		i++;
	}
}

void ServerRun::dataIn(s_poll_data pollData, struct pollfd pollFd, int idx)
{
	switch (pollData.pollType)
	{
		case LISTENER:
			acceptNewConnection(pollFd.fd, pollData.serverNum);
			break ;
		case CLIENT_CONNECTION: 
			readRequest(pollFd.fd); // TODO this READS and WRITES the request and closes connection... we need to separate this 
			removeConnection(idx);
			break ;
		case STATIC_FILE:
			break ;
		default:
			break;
	}
}

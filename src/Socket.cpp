#include "Socket.hpp"
#include <string>

void Socket::fillStruct(struct addrinfo& hints) {
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = 0;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // fills in your local host ip for you, saves you from having to hard code it
}

Socket::Socket(std::string name)
{
	struct addrinfo hints, * res, * tmp;
	fillStruct(hints);
	int status = getaddrinfo(name.c_str(), "http", &hints, &res);
	if (status != 0)
	{
		throw (Exception("Error with getaddrinfo()", errno));
	}
	bool bound = false;
	for (tmp = res; tmp != nullptr; tmp = tmp->ai_next)
	{
		_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (_fd == -1)
			continue;
		//Check the code below for opti. It makes socket reusable so no waits
		int opt = 1;
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "Failed to set socket options" << std::endl;
			close(_fd);
			freeaddrinfo(res);
			throw (Exception("Failed to set socket options", 1));
		}
		fcntl(_fd, F_SETFL, O_NONBLOCK);
		if (bind(_fd, res->ai_addr, res->ai_addrlen) == 0)
		{
			bound = true;
			break;
		}
		close(_fd);
	}
	freeaddrinfo(res);
	if (!bound)
		throw (Exception("Socket failed to bind :(\t" + name, 1));
	if (listen(_fd, SOMAXCONN))
	{
		close(_fd);
		throw (Exception("listening failed on socketfd fd: " + std::to_string(_fd) + " on  ): " + name, 1));
	}
	//Logger::log("Sockets created with fd " + std::to_string(_fd) + " on port: " + std::to_string(port));
}

Socket::Socket(s_domain domain)
{
	struct addrinfo hints, * res, * tmp;
	std::string	port_str = std::to_string(domain.port);
	fillStruct(hints);
	int status = getaddrinfo(domain.host.c_str(), port_str.c_str(), &hints, &res);
	bool bound = false;

	if (status != 0)
		throw (Exception("Error with getaddrinfo()", errno));

	for (tmp = res; tmp != nullptr; tmp = tmp->ai_next)
	{
		_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (_fd == -1)
			continue;
		//Check the code below for opti. It makes socket reusable so no waits
		int opt = 1;
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "Failed to set socket options" << std::endl;
			close(_fd);
			freeaddrinfo(res);
			throw (Exception("Failed to set socket options", 1));
		}
		fcntl(_fd, F_SETFL, O_NONBLOCK);
		if (bind(_fd, res->ai_addr, res->ai_addrlen) == 0)
		{
			bound = true;
			break;
		}
		close(_fd);
	}
	freeaddrinfo(res);
	if (!bound)
		throw (Exception("Socket failed to bind, port: " + std::to_string(domain.port), 1));
	if (listen(_fd, SOMAXCONN))
	{
		close(_fd);
		throw (Exception("listening failed on socketfd fd: " + std::to_string(_fd) + " on port: " + std::to_string(domain.port), 1));
	}
	//Logger::log("Sockets created with fd " + std::to_string(_fd) + " on port: " + std::to_string(port));
}

Socket::~Socket()
{
	if (_fd != -1)
		close(_fd);
}

Socket& Socket::operator=(Socket& other) noexcept
{
	_fd = other._fd;
	other._fd = -1;
	return (*this);
}

// Getters

int	Socket::getFd(void)
{
	return (_fd);
}

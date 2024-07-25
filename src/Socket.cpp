#include "Socket.hpp"

Socket::Socket(int port)
{
	struct addrinfo hints, *res, *tmp;
	std::string port_str = std::to_string(port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = 0;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // fills in your local host ip for you, saves you from having to hard code it

	// TODO add IP number as argument one. see if getaddrinfo accepts IPv4
	//127.0.0.1 - 127.255.255.254
	int status = getaddrinfo(NULL, port_str.c_str(), &hints, &res);
	bool bound = false;
	if(status != 0)
	{
		throw(Exception("Error with getaddrinfo()", errno));
	}
	for (tmp = res; tmp != nullptr; tmp = tmp->ai_next)
	{
		_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (_fd == -1)
			continue ;

		//Check the code below for opti. It makes socket reusable so no waits
		int opt = 1;
		if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "Failed to set socket options" << std::endl;
			close(_fd);
			freeaddrinfo(res);
			throw(Exception("Failed to set socket options", errno));
		}

		fcntl(_fd, F_SETFL, O_NONBLOCK);
		if (bind(_fd, res->ai_addr, res->ai_addrlen) == 0)
		{
			bound = true;
			break ;
		}
		close(_fd);
	}
	freeaddrinfo(res);
	if (!bound)
	{
		std::cout << "Port " << port << ": ";
		throw(Exception("Socket failed to bind", errno));
	}
	if (listen(_fd, SOMAXCONN))
	{
		close(_fd);
		std::cout << "listening failed on socketfd " << _fd << " on port " << port << std::endl; 
	}
	std::cout << "Sockets created with fd " << _fd << " on: " << port << std::endl;
}
	
Socket::~Socket()
{
	if (_fd != -1)
		close(_fd);
}

Socket &Socket::operator=(Socket &other) noexcept
{
	_fd = other._fd;
	other._fd = -1;
	return (*this);
}

// Getters

int	Socket::getFd( void )
{
	return (_fd);
}

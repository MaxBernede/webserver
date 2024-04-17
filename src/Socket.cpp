#include "Socket.hpp"
#include "webserver.hpp"

Socket::Socket(int port)
{
	struct addrinfo hints, *res, *tmp;
	std::string port_str = std::to_string(port);

	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = 0;
	hints.ai_family = AF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE; // fills in your local host ip for you, saves you from having to hard code it

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
		std::cout << "binding socket " << port << " failed" << std::endl;
		throw(Exception("Socket failed to bind", errno));
	}
	if (listen(_fd, SOMAXCONN))
	{
		close(_fd);
		std::cout << "listening failed on socketfd " << _fd << " on port " << port << std::endl; 
	}
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


/*
int accept(int sockfd, struct sockaddr *cli_addr, int addr_len)
accept new connections from new clients
upon success, the accept() function returns a fd for the accepted socket (a non-negative integer)
upon error, returns -1 and sets errno to indicate the error 
*/

// Getters

int	Socket::getFd( void )
{
	return (_fd);
}

#include "Socket.hpp"
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <cstring>

/*
AF_INET/AF_INET: specifies address family IPv4/IPv6
SOCK_STREAM: indicates a TCP socket type
O_NONBLOCK: makes the socket non-blocking, meaning that socket operations (e.g., read(), write()) won't block the program's execution.
O_CLOEXEC: makes the file descriptor close-on-exec, meaning that the socket will be closed automatically when a new executable is loaded using exec() system call.
IPPROTO_TCP :specifies the protocol to be used, in this case, TCP.
*/
Socket::Socket(s_port port)
{
	struct addrinfo *hint, *res, *tmp;
	std::string port_str = std::to_string(port.nmb);

	memset(&hints, 0, sizeof(hints));
	hints->ai_family = AF_UNSPEC;
	hints->ai_socktype = 0;
	hints->ai_protocol = IPPROTO_TCP;
	hints->ai_flags = AI_PASSIVE; // fills in your local host ip for you, saves you from having to hard code it
	
	int status = getaddrinfo(NULL, port_str.c_str(), hints, &res);
	bool bound = false;
	if(status != 0)
	{
		throw(Exception("Error with getaddrinfo()", errno));
	}
	for (tmp = hints; tmp != nullptr; tmp = tmp->ai_next)
	{
		_fd = socket(tmp->ai_family, tmp->ai_socktype, tmp->ai_protocol);
		if (_fd == -1)
			continue ; //move to the next iteration of the loop
		//throw(Exception("failed to open socket", errno));
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
		throw(Exception("Socket failed to bind", errno));
	if (listen(_fd, SOMAXCONN))
	{
		close(_fd);
		std::cout << "listening failed on socketfd " << _fd << " on port " << port.nmb << ", type " << port.type << std::endl; 
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

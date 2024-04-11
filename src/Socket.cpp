#include "../inc/webserver.hpp"
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

// AF_INET/AF_INET: specifies address family IPv4/IPv6
// SOCK_STREAM: indicates a TCP socket type
// O_NONBLOCK: makes the socket non-blocking, meaning that socket operations (e.g., read(), write()) won't block the program's execution.
// O_CLOEXEC: makes the file descriptor close-on-exec, meaning that the socket will be closed automatically when a new executable is loaded using exec() system call.
// IPPROTO_TCP :specifies the protocol to be used, in this case, TCP.
Socket::Socket()
{
	// 3rd parameter, type can also be 0 since this refers to default, and the default for SOCK_STREAM is TCP
	_fd = socket(AF_INET | AF_INET6, SOCK_STREAM | O_NONBLOCK | O_CLOEXEC, IPPROTO_TCP);
	if (_fd == -1)
		throw(Exception("failed to open socket", errno));
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
	_addr = other._addr;
	return (*this);
}

// :: tells the compiler to look for the instance of listen in the global namespace rather than in the current namespace/class/scope
// backlog is passed from when RunServer object creates an instance of a Socket (value is SOMAXCONN = largest value possible = 128, if a larger value is used, it is truncated to this value)
// listen(int fd, int backlog) marks the socket referred to by sopckfd, as a socket that will be used to accept incoming connetion requests using accept
// the parameter fd refers to the fd returned by by socket() of type SOCK_STREAM or SOCK_SEQPACKET
// on success, listen returns 0, else it returns -1 and sets errno
void Socket::listen(int backlog) const 
{
	if (::listen(_fd, backlog) == -1)
		throw(Exception("failed to listen", errno));
}

// bind the socket to an IP and port (i think the port number comes from the config file)
// client calls connect() while server calls bind(), listen(), accept()
// send() and close() is used by both the client and server sides
// bind(int sockfd, struct sockaddr *serv_addr, int addrlen)
// sockfd = fd returned by socket() | serv_addr = contains server IP address and port | addrlen = length of the address in bytes 
void Socket::bind(uint16_t port) const
{
	struct addrinfo *serv_addr;
	struct addrinfo *hints;

	std::string port_str = std::to_string(port);
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_protocol = IPPROTO_TCP;
	// setting ai_flags to AI_PASSIVE (together with node = NULL) as this returns a socket address suitable for bind() that will accept connections
	// the returned socket address will contain the wildcard address which is used by applications that intend to accept connections. If node (when calling getaddrinfo()) is not NULL, then AI_PASSIVE is ignored
	hints->ai_flags = AI_PASSIVE;
	int status = getaddrinfo(NULL, port_str.c_str(), hints, &serv_addr);
	if(status != 0)
	{
		std::cerr << "Error with getaddrinfo() in bind() function" << std::endl;
		throw(Exception("Error with getaddrinfo() in bind() function", errno));
	}
	// getaddrinfo() returns a list of addrinfo() structs - you need to loop through the linked list
	addrinfo *i = serv_addr;
	bool bind = false;
	while (i != NULL)
	{
		if (::bind(_fd, serv_addr->ai_addr, serv_addr->ai_addrlen) == 0)
			bind = true;
		i = i->ai_next;
	}
	freeaddrinfo(serv_addr);
	if (!bind)
		throw(Exception("Socket failed to bind", errno));
}


// int accept(int sockfd, struct sockaddr *cli_addr, int addr_len)
// accept new connections from new clients
// upon success, the accept() function returns a fd for the accepted socket (a non-negative integer)
// upon error, returns -1 and sets errno to indicate the error
Socket Socket::accept() const
{
	struct sockaddr_in cli_addr = {};
	int len = sizeof(sockaddr_in);
	// int acc_fd = accept(_fd, &cli_addr, len);
	// if (acc_fd == -1)
	// 	throw(Exception("accept() error and returned -1", errno));
}


// Getters

int	Socket::getFd( void )
{
	return (_fd);
}

std::string &Socket::getAddr( void )
{
	return (_addr);
}

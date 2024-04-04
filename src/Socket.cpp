#include "Socket.hpp"

// AF_INET: specifies address family as IPv4
// SOCK_STREAM: indicates a TCP socket type
// O_NONBLOCK: makes the socket non-blocking, meaning that socket operations (e.g., read(), write()) won't block the program's execution.
// O_CLOEXEC: makes the file descriptor close-on-exec, meaning that the socket will be closed automatically when a new executable is loaded using exec() system call.
// IPPROTO_TCP :specifies the protocol to be used, in this case, TCP.
Socket::Socket()
{
	_fd = socket(AF_INET, SOCK_STREAM | O_NONBLOCK | O_CLOEXEC, IPPROTO_TCP);
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
void Socket::listen(int backlog) const 
{
	if (::listen(_fd, backlog) == -1)
		throw(Exception("failed to listen", errno));
}

void Socket::bind(uint16_t port) const
{

}

Socket Socket::accept() const
{
	
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